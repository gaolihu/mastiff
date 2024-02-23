/*
 * @Date: 2023-11-20 20:13:03
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera.h"

#include <unistd.h>

#include "cyber/common/log.h"
#include "cyber/cyber.h"

Camera::Camera(AS_CAM_PTR pCamera, const AS_SDK_CAM_MODEL_E &cam_type) {
    int ret     = 0;
    m_handle    = pCamera;
    m_cam_type  = cam_type;
    m_check_fps = new CheckFps(pCamera);
    ret         = AS_SDK_GetCameraAttrs(m_handle, m_attr);
    if (ret != 0) {
        AWARN << "get camera attrs failed";
    }
    memset(&m_cam_parameter, 0, sizeof(AS_CAM_Parameter_s));
}

Camera::~Camera() {
    if (m_check_fps != nullptr) {
        delete m_check_fps;
        m_check_fps = nullptr;
    }
    if (m_is_thread) {
        m_is_thread = false;
    }
    if (m_backgroundThread.joinable()) {
        m_backgroundThread.join();
    }
}

int Camera::init() {
    int  ret         = 0;
    char sn_buff[64] = {0};
    ret              = AS_SDK_GetSerialNumber(m_handle, sn_buff, sizeof(sn_buff));
    if (ret != 0) {
        AFATAL << "get camera serial number failed" << std::endl;
        return -1;
    }
    m_serialno = std::string(sn_buff);

    char fwVersion[100] = {0};
    ret                 = AS_SDK_GetFwVersion(m_handle, fwVersion, sizeof(fwVersion));
    if (ret == 0) {
        AINFO << "#camera[" << m_handle << "] SN[" << m_serialno << "]'s firmware version:" << fwVersion << std::endl;
    }
    m_is_thread        = true;
    m_backgroundThread = std::thread(&Camera::backgroundThread, this);
    return ret;
}
void Camera::Stop() {}
void Camera::Close() {}
void Camera::Restart() {}

double Camera::checkFps() {
    std::string Info = "";
    switch (m_attr.type) {
    case AS_CAMERA_ATTR_LNX_USB:
        Info = (std::to_string(m_attr.attr.usbAttrs.bnum) + ":" + m_attr.attr.usbAttrs.port_numbers);
        break;
    case AS_CAMERA_ATTR_NET:
        Info = (std::to_string(m_attr.attr.netAttrs.port) + ":" + m_attr.attr.netAttrs.ip_addr);
        break;
    default:
        AFATAL << "attr type error" << std::endl;
        break;
    }
    return m_check_fps->checkFps(m_serialno, Info);
}

int Camera::enableSaveImage(bool enable) {
    m_save_img = enable;
    if (m_cam_type == AS_SDK_CAM_MODEL_KUNLUN_A) {
        m_save_merge_img = enable;
    }
    return 0;
}
int Camera::enableSaveImageToFile(bool enable) {
    save_to_file_ = enable;
    return 0;
}

int Camera::enableDisplay(bool enable) {
    m_display = enable;
    if (m_cam_type == AS_SDK_CAM_MODEL_KUNLUN_A) {
        m_display_merge = enable;
    }
    return 0;
}

int Camera::getSerialNo(std::string &sn) {
    sn = m_serialno;
    return 0;
}

int Camera::getCameraAttrs(AS_CAM_ATTR_S &info) {
    info = m_attr;
    return 0;
}

int Camera::backgroundThread() {
    int ret = 0;
    while (m_is_thread) {
        // KONDYOR not support to get CamParameter, KUNLUN A don't need to get CamParameter
        if ((m_cam_type != AS_SDK_CAM_MODEL_KONDYOR_NET) && (m_cam_type != AS_SDK_CAM_MODEL_KONDYOR) && (m_cam_type != AS_SDK_CAM_MODEL_KUNLUN_A)) {
            ret = AS_SDK_GetCamParameter(m_handle, &m_cam_parameter);
            if (ret == 0) {
                AINFO << "SN [ " << m_serialno << " ]'s parameter:" << std::endl;
                AINFO << "irfx: " << m_cam_parameter.fxir << std::endl;
                AINFO << "irfy: " << m_cam_parameter.fyir << std::endl;
                AINFO << "ircx: " << m_cam_parameter.cxir << std::endl;
                AINFO << "ircy: " << m_cam_parameter.cyir << std::endl;
                AINFO << "rgbfx: " << m_cam_parameter.fxrgb << std::endl;
                AINFO << "rgbfy: " << m_cam_parameter.fyrgb << std::endl;
                AINFO << "rgbcx: " << m_cam_parameter.cxrgb << std::endl;
                AINFO << "rgbcy: " << m_cam_parameter.cyrgb << std::endl << std::endl;
                m_is_thread = false;
                break;
            }
        }
        else {
            m_is_thread = false;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}
void Camera::PublishAnImageRgb(const AS_Frame_s &img_data) {
    callback_finished_ = false;
    //header
    img_rgb_.mutable_header()->set_seq(frame_seq_);
    img_rgb_.mutable_header()->set_frame_id(frame_id_);
    auto sec  = cyber::Time::Now().ToSecond();
    auto nsec = cyber::Time::Now().ToNanosecond() % static_cast<uint64_t>(1e9);
    img_rgb_.mutable_header()->mutable_stamp()->set_sec(sec);
    img_rgb_.mutable_header()->mutable_stamp()->set_nsec(nsec);

    // data
    img_rgb_.set_width(img_data.width);
    img_rgb_.set_height(img_data.height);
    img_rgb_.set_encoding("16UC1");
    img_rgb_.set_is_bigendian(false);
    img_rgb_.set_step(img_data.width * 2);

    /*
    AINFO << "Rgb width: " << img_data.width <<
        ", height: " << img_data.height <<
        ", buf size " << img_data.bufferSize <<
        ", img size: " << img_data.size;
        */

    // image buffer
    size_t             len = img_data.size;
    std::string data_str;
    {
        data_str.clear();
        data_str = std::string(static_cast<const char *>(img_data.data), len);
    }
    img_rgb_.set_data(data_str);
}

void Camera::PublishAnImageDepth(const AS_Frame_s &img_data) {
    callback_finished_ = false;
    //header
    img_depth_.mutable_header()->set_seq(frame_seq_);
    img_depth_.mutable_header()->set_frame_id(frame_id_);
    auto sec  = cyber::Time::Now().ToSecond();
    auto nsec = cyber::Time::Now().ToNanosecond() % static_cast<uint64_t>(1e9);
    img_depth_.mutable_header()->mutable_stamp()->set_sec(sec);
    img_depth_.mutable_header()->mutable_stamp()->set_nsec(nsec);

    // data
    img_depth_.set_width(img_data.width);
    img_depth_.set_height(img_data.height);
    img_depth_.set_encoding("16UC1");
    img_depth_.set_is_bigendian(false);
    img_depth_.set_step(img_data.width * 2);

    /*
    AINFO << "dep width: " << img_data.width <<
        ", height: " << img_data.height <<
        ", buf size " << img_data.bufferSize <<
        ", img size: " << img_data.size;
        */

    // image buffer
    size_t             len = img_data.size;
    std::string data_str;
    {
        data_str.clear();
        data_str = std::string(static_cast<const char *>(img_data.data), len);
    }
    img_depth_.set_data(data_str);
}

void Camera::PublishAPointCloud(const AS_Frame_s &pc_data) {
    //header
    pcl_.mutable_header()->set_seq(frame_seq_);
    pcl_.mutable_header()->set_frame_id(frame_id_);
    auto sec  = cyber::Time::Now().ToSecond();
    auto nsec = cyber::Time::Now().ToNanosecond() % static_cast<uint64_t>(1e9);
    pcl_.mutable_header()->mutable_stamp()->set_sec(sec);
    pcl_.mutable_header()->mutable_stamp()->set_nsec(nsec);

    // meta data
    pcl_.set_width(pc_data.size / sizeof(float) / 3);
    pcl_.set_height(1);
    pcl_.set_point_step(sizeof(float) * 3);
    pcl_.set_row_step(pc_data.size);
    pcl_.set_is_dense(true);
    pcl_.set_is_bigendian(false);

    //field
    auto field1 = pcl_.add_fields();

    std::string x_name = "x", y_name = "y", z_name = "z";
    field1->set_name(x_name);
    field1->set_offset(0);
    field1->set_datatype(sensor_msgs::PointField::TYPE::PointField_TYPE_FLOAT32);
    field1->set_count(1);
    auto field2 = pcl_.add_fields();
    field2->set_name(y_name);
    field2->set_offset(4);
    field2->set_datatype(sensor_msgs::PointField::TYPE::PointField_TYPE_FLOAT32);
    field2->set_count(1);
    auto field3 = pcl_.add_fields();
    field3->set_name(z_name);
    field3->set_offset(8);
    field3->set_datatype(sensor_msgs::PointField::TYPE::PointField_TYPE_FLOAT32);
    field3->set_count(1);

    size_t points     = pcl_.width() * pcl_.height() * 3;
    size_t points_len = points * sizeof(float);

    std::vector<float> pc_tmp;
    pc_tmp.clear();
    pc_tmp.resize(points);

    for (size_t i = 0; i < pc_tmp.size(); i++) {
        pc_tmp[i] = *((float *)(pc_data.data) + i) / 1000.0;
    }

    std::string pc2_data;
    {
        pc2_data.clear();
        pc2_data = std::string(reinterpret_cast<const char *>(pc_tmp.data()), points_len);
    }
    pcl_.set_data(pc2_data);

    // send
    //msg_publisher_(pcl_);
    frame_seq_++;
    //AINFO << "PublishAPointCloud PCL cnt: " << frame_seq_;
}

void Camera::saveImage(const AS_SDK_Data_s *pstData) {
    std::lock_guard<std::mutex> lg(pub_mtx_);

    if (!callback_finished_) {
        return;
    }
    callback_finished_ = false;

    if (!pstData)
        return;

    if (!m_save_img) {
        m_cnt = 0;
        return;
    }

    if (m_cam_type == AS_SDK_CAM_MODEL_KUNLUN_A) {
        if (m_cnt >= 1) {
            m_save_img = false;
        }
        m_cnt++;
    }
    else {
        m_save_img = false;
    }

    sdk_data_ = *pstData;

    if (sdk_data_.depthImg.size > 0) {
        std::string depthimgName(std::string(m_serialno + "_depth_") + std::to_string(sdk_data_.depthImg.width) + "x"
                                 + std::to_string(sdk_data_.depthImg.height) + "_" + std::to_string(m_depthindex++) + ".yuv");
        if (save_to_file_) {
            auto ret = saveYUVImg(depthimgName.c_str(), sdk_data_.depthImg.data, sdk_data_.depthImg.size);
            if (ret != 0) {
                AFATAL << "save depth image failed!" << std::endl;
            }
            else {
                AINFO << "save depth image success!" << std::endl;
                AINFO << "location: " << getcwd(nullptr, 0) << "/" << depthimgName << std::endl;
            }
        }
        else {
            PublishAnImageDepth(sdk_data_.depthImg);
        }
    }

    if (sdk_data_.rgbImg.size > 0) {
        if (save_to_file_) {
            std::string rgbName(std::string(m_serialno + "_rgb_") + std::to_string(sdk_data_.rgbImg.width) + "x"
                                + std::to_string(sdk_data_.rgbImg.height) + "_" + std::to_string(m_rgbindex++) + ".yuv");
            auto        ret = saveYUVImg(rgbName.c_str(), sdk_data_.rgbImg.data, sdk_data_.rgbImg.size);
            if (ret != 0) {
                AFATAL << "save rgb image failed!" << std::endl;
            }
            else {
                AINFO << "save rgb image success!" << std::endl;
                AINFO << "location: " << getcwd(nullptr, 0) << "/" << rgbName << std::endl;
            }
        }
        else {
            PublishAnImageRgb(sdk_data_.rgbImg);
        }
    }

    if (sdk_data_.pointCloud.size > 0) {
        if (save_to_file_) {
            std::string pointCloudName(std::string(m_serialno + "_PointCloud_" + std::to_string(sdk_data_.pointCloud.width) + "x"
                                                   + std::to_string(sdk_data_.pointCloud.height) + "_" + std::to_string(m_pointCloudIndex++)
                                                   + ".pcd"));
            if (savePointCloudWithPcdFormat(pointCloudName.c_str(),
                                            static_cast<float *>(sdk_data_.pointCloud.data),
                                            sdk_data_.pointCloud.size / sizeof(float))
                != 0) {
                AFATAL << "save point cloud failed!" << std::endl;
            }
            else {
                AINFO << "save point cloud success!" << std::endl;
                AINFO << "location: " << getcwd(nullptr, 0) << "/" << pointCloudName << std::endl;
            }
        }
        else {
            PublishAPointCloud(sdk_data_.pointCloud);
        }
    }

    has_image_ = true;
    callback_finished_ = true;
    save_to_file_      = false;
    return;
}

void Camera::saveMergeImage(const AS_SDK_MERGE_s *pstData) {
    if (!m_save_merge_img) {
        return;
    }
    m_save_merge_img = false;
    if (sdk_data_.depthImg.size > 0) {
        std::string depthimgName(std::string(m_serialno + "_depth_merge_") + std::to_string(sdk_data_.depthImg.width) + "x"
                                 + std::to_string(sdk_data_.depthImg.height) + "_" + std::to_string(m_depthindex++) + ".yuv");
        if (saveYUVImg(depthimgName.c_str(), sdk_data_.depthImg.data, sdk_data_.depthImg.size) != 0) {
            AFATAL << "save depth image failed!" << std::endl;
        }
        else {
            AINFO << "save depth image success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << depthimgName << std::endl;
        }
    }

    if (sdk_data_.pointCloud.size > 0) {
        std::string pointCloudName(std::string(m_serialno + "_PointCloud_merge_" + std::to_string(sdk_data_.pointCloud.width) + "x"
                                               + std::to_string(sdk_data_.pointCloud.height) + "_" + std::to_string(m_pointCloudIndex++) + ".pcd"));
        if (savePointCloudWithPcdFormat(pointCloudName.c_str(),
                                        static_cast<float *>(sdk_data_.pointCloud.data),
                                        sdk_data_.pointCloud.size / sizeof(float))
            != 0) {
            AFATAL << "save point cloud failed!" << std::endl;
        }
        else {
            AINFO << "save point cloud success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << pointCloudName << std::endl;
        }
    }

    return;
}

void Camera::depth2color(cv::Mat &color, const cv::Mat &depth, const double max, const double min) {
    cv::Mat grayImage;
    double  alpha = 255.0 / (max - min);
    depth.convertTo(grayImage, CV_8UC1, alpha, -alpha * min);
    cv::applyColorMap(grayImage, color, cv::COLORMAP_JET);
}

cv::Mat Camera::yuyv2bgr(const cv::Mat &yuyv) {
    CV_Assert(yuyv.type() == CV_8UC2);
    cv::Mat bgr;
    cv::cvtColor(yuyv, bgr, cv::COLOR_YUV2BGR_YUYV);
    return bgr;
}

bool Camera::getDisplayStatus() {
    if (m_cam_type == AS_SDK_CAM_MODEL_KUNLUN_A) {
        if ((m_display == false) && (m_display_merge == false)) {
            return false;
        }
        else if ((m_display == true) && (m_display_merge == true)) {
            return true;
        }
        else {
            return true;
        }
    }
    else {
        return m_display;
    }
}

bool Camera::GetImageDatas(mstf::chss::proto::CameraPopDatas& data) {
    if (!has_image_) {
        return false;
    }

    std::lock_guard<std::mutex> lg(pub_mtx_);
    data.mutable_rgb()->CopyFrom(img_rgb_);
    data.mutable_depth()->CopyFrom(img_depth_);
    data.mutable_pcl()->CopyFrom(pcl_);
    has_image_ = false;
    return true;
}

/**
 * @file      Camera.cpp
 * @brief     angstrong Camera.
 *
 * Copyright (c) 2023 Angstrong Tech.Co.,Ltd
 *
 * @author    Angstrong SDK develop Team
 * @date      2023/05/12
 * @version   1.0

 */
#include <unistd.h>
#include "cyber/cyber.h"
#include "cyber/common/log.h"
#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera.h"
#include "modules/aventurier_common_msgs/proto/ros_msgs/sensor_msgs.pb.h"

using namespace ventura;
using namespace common_msgs;
using namespace sensor_msgs;

Camera::Camera(AS_CAM_PTR pCamera, const AS_SDK_CAM_MODEL_E &cam_type) {
    int ret     = 0;
    m_handle    = pCamera;
    m_cam_type  = cam_type;
    m_check_fps = new CheckFps(pCamera);
    ret         = AS_SDK_GetCameraAttrs(m_handle, m_attr);
    if (ret != 0) {
        AWARN << "get camera attrs failed" ;
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

void Camera::saveImage(const AS_SDK_Data_s *pstData) {
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

    if (pstData->depthImg.size > 0) {
        std::string depthimgName(std::string(m_serialno + "_depth_") + std::to_string(pstData->depthImg.width) + "x"
                                 + std::to_string(pstData->depthImg.height) + "_" + std::to_string(m_depthindex++) + ".yuv");
        if (saveYUVImg(depthimgName.c_str(), pstData->depthImg.data, pstData->depthImg.size) != 0) {
            AFATAL << "save depth image failed!" << std::endl;
        }
        else {
            if(soc_listener_){
                auto response = sensor_msgs::Image();
                response.set_width(pstData->depthImg.width);
                response.set_height(pstData->depthImg.height);
                response.set_encoding("16UC1");
                response.set_is_bigendian(false);
                response.set_step(pstData->depthImg.width/2);

                std::vector<unsigned char> vcTmp((unsigned char *)(pstData->depthImg.data),
                        //!!! FIXME Glh 2023/11/7 15:11 !!!
                                     (unsigned char *)(static_cast<int*>(pstData->depthImg.data) + pstData->depthImg.size));
                response.mutable_data()->assign(vcTmp.begin(), vcTmp.end());
                soc_listener_(response);
            }
            AINFO << "save depth image success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << depthimgName << std::endl;
        }
    }

    if (pstData->rgbImg.size > 0) {
        std::string rgbName(std::string(m_serialno + "_rgb_") + std::to_string(pstData->rgbImg.width) + "x" + std::to_string(pstData->rgbImg.height)
                            + "_" + std::to_string(m_rgbindex++) + ".yuv");
        if (saveYUVImg(rgbName.c_str(), pstData->rgbImg.data, pstData->rgbImg.size) != 0) {
            AFATAL << "save rgb image failed!" << std::endl;
        }
        else {
            if(soc_listener_){
                auto response = sensor_msgs::Image();
                response.set_width(pstData->rgbImg.width);
                response.set_height(pstData->rgbImg.height);
                response.set_encoding("16UC1");
                response.set_is_bigendian(false);
                response.set_step(pstData->rgbImg.width/2);

                        //!!! FIXME Glh 2023/11/7 15:12 !!!
                std::vector<unsigned char> vcTmp((unsigned char *)(pstData->rgbImg.data),
                                     (unsigned char *)(static_cast<int*>(pstData->rgbImg.data) + pstData->rgbImg.size));
                response.mutable_data()->assign(vcTmp.begin(), vcTmp.end());
                soc_listener_(response);
            }
            AINFO << "save rgb image success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << rgbName << std::endl;
        }
    }

    if (pstData->pointCloud.size > 0) {
        std::string pointCloudName(std::string(m_serialno + "_PointCloud_" + std::to_string(pstData->pointCloud.width) + "x"
                                               + std::to_string(pstData->pointCloud.height) + "_" + std::to_string(m_pointCloudIndex++) + ".pcd"));
        if (savePointCloudWithPcdFormat(pointCloudName.c_str(),
                                        static_cast<float *>(pstData->pointCloud.data),
                                        pstData->pointCloud.size / sizeof(float))
            != 0) {
            AFATAL << "save point cloud failed!" << std::endl;
        }
        else {
            AINFO << "save point cloud success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << pointCloudName << std::endl;
        }
    }

    if (pstData->irImg.size > 0) {
        std::string irimgName(std::string(m_serialno + "_ir_" + std::to_string(pstData->irImg.width) + "x" + std::to_string(pstData->irImg.height)
                                          + "_" + std::to_string(m_irindex++) + ".yuv"));
        if (saveYUVImg(irimgName.c_str(), pstData->irImg.data, pstData->irImg.size) != 0) {
            AFATAL << "save ir image failed!" << std::endl;
        }
        else {
            AINFO << "save ir image success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << irimgName << std::endl;
        }
    }

    if (pstData->peakImg.size > 0) {
        std::string peakimgName(std::string(m_serialno + "_peak_") + std::to_string(pstData->peakImg.width) + "x"
                                + std::to_string(pstData->peakImg.height) + "_" + std::to_string(m_peakindex++) + ".yuv");
        if (saveYUVImg(peakimgName.c_str(), pstData->peakImg.data, pstData->peakImg.size) != 0) {
            AFATAL << "save peak image failed!" << std::endl;
        }
        else {
            AINFO << "save peak image success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << peakimgName << std::endl;
        }
    }

    if (pstData->yuyvImg.size > 0) {
        std::string yuyvName(std::string(m_serialno + "_yuyv_") + std::to_string(pstData->yuyvImg.width) + "x"
                             + std::to_string(pstData->yuyvImg.height) + "_" + std::to_string(m_yuyvindex++) + ".yuv");
        if (saveYUVImg(yuyvName.c_str(), pstData->yuyvImg.data, pstData->yuyvImg.size) != 0) {
            AFATAL << "save yuyv image failed!" << std::endl;
        }
        else {
            AINFO << "save yuyv image success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << yuyvName << std::endl;
        }
    }

    return;
}

void Camera::saveMergeImage(const AS_SDK_MERGE_s *pstData) {
    if (!m_save_merge_img) {
        return;
    }
    m_save_merge_img = false;
    if (pstData->depthImg.size > 0) {
        std::string depthimgName(std::string(m_serialno + "_depth_merge_") + std::to_string(pstData->depthImg.width) + "x"
                                 + std::to_string(pstData->depthImg.height) + "_" + std::to_string(m_depthindex++) + ".yuv");
        if (saveYUVImg(depthimgName.c_str(), pstData->depthImg.data, pstData->depthImg.size) != 0) {
            AFATAL << "save depth image failed!" << std::endl;
        }
        else {
            AINFO << "save depth image success!" << std::endl;
            AINFO << "location: " << getcwd(nullptr, 0) << "/" << depthimgName << std::endl;
        }
    }

    if (pstData->pointCloud.size > 0) {
        std::string pointCloudName(std::string(m_serialno + "_PointCloud_merge_" + std::to_string(pstData->pointCloud.width) + "x"
                                               + std::to_string(pstData->pointCloud.height) + "_" + std::to_string(m_pointCloudIndex++) + ".pcd"));
        if (savePointCloudWithPcdFormat(pointCloudName.c_str(),
                                        static_cast<float *>(pstData->pointCloud.data),
                                        pstData->pointCloud.size / sizeof(float))
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

void Camera::displayImage(const std::string &serialno, const std::string &info, const AS_SDK_Data_s *pstData) {
#ifdef CFG_OPENCV_ON
    if (m_display) {
        if (pstData->irImg.size > 0) {
            cv::Mat IrImage = cv::Mat(pstData->irImg.height, pstData->irImg.width, CV_8UC1, pstData->irImg.data);
            cv::imshow(serialno + "_" + info + "_ir_" + std::to_string(pstData->irImg.width) + "x" + std::to_string(pstData->irImg.height), IrImage);
        }

        if (pstData->depthImg.size > 0) {
            cv::Mat depthImage = cv::Mat(pstData->depthImg.height, pstData->depthImg.width, CV_16UC1, pstData->depthImg.data);
            cv::Mat depth_img_pseudo_color;

            double minVal;
            double maxVal;
            cv::minMaxIdx(depthImage, &minVal, &maxVal);
            depth2color(depth_img_pseudo_color, depthImage, maxVal, minVal);
            cv::imshow(serialno + "_" + info + "_depth_" + std::to_string(pstData->depthImg.width) + "x" + std::to_string(pstData->depthImg.height),
                       depth_img_pseudo_color);
        }

        if (pstData->rgbImg.size > 0) {
            cv::Mat rgbImage = cv::Mat(pstData->rgbImg.height, pstData->rgbImg.width, CV_8UC3, pstData->rgbImg.data);
            cv::imshow(serialno + "_" + info + "_rgb_" + std::to_string(pstData->rgbImg.width) + "x" + std::to_string(pstData->rgbImg.height),
                       rgbImage);
        }

        if (pstData->peakImg.size > 0) {
            cv::Mat peakImg = cv::Mat(pstData->peakImg.height, pstData->peakImg.width, CV_8UC1, pstData->peakImg.data);
            cv::imshow(serialno + std::string("_peak_") + std::to_string(pstData->peakImg.width) + "x" + std::to_string(pstData->peakImg.height),
                       peakImg);
        }

        if (pstData->yuyvImg.size > 0) {
            cv::Mat yuyv    = cv::Mat(pstData->yuyvImg.height, pstData->yuyvImg.width, CV_8UC2, pstData->yuyvImg.data);
            cv::Mat yuyvImg = yuyv2bgr(yuyv);
            cv::imshow(serialno + "_yuyv_" + std::to_string(pstData->yuyvImg.width) + "x" + std::to_string(pstData->yuyvImg.height), yuyvImg);
        }

        cv::waitKey(3);
    }
    else {
        cv::destroyAllWindows();
    }
#endif
    return;
}

void Camera::displayMergeImage(const std::string &serialno, const std::string &info, const AS_SDK_MERGE_s *pstData) {
#ifdef CFG_OPENCV_ON
    if (m_display_merge) {
        if (pstData->depthImg.size > 0) {
            cv::Mat depthImage = cv::Mat(pstData->depthImg.height, pstData->depthImg.width, CV_16UC1, pstData->depthImg.data);
            cv::Mat depth_img_pseudo_color;

            double minVal;
            double maxVal;
            cv::minMaxIdx(depthImage, &minVal, &maxVal);
            depth2color(depth_img_pseudo_color, depthImage, maxVal, minVal);
            cv::imshow(
                serialno + "_" + info + "_depth_merge_" + std::to_string(pstData->depthImg.width) + "x" + std::to_string(pstData->depthImg.height),
                depth_img_pseudo_color);
        }

        cv::waitKey(3);
    }
    else {
        cv::destroyAllWindows();
    }
#endif
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

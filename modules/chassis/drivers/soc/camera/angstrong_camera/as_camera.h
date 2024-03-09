#pragma once
#include <chrono>
#include <cstring>
#include <mutex>
#include <sstream>
#include <thread>

#include "as_camera_sdk_api.h"
#include "common.h"
#include "cyber/common/log.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include "modules/chassis/proto/chss_io.pb.h"

using namespace apollo;
using namespace ventura;
using namespace common_msgs;
using namespace sensor_msgs;

class CheckFps {
public:
    CheckFps(AS_CAM_PTR pCamera) {
        m_pCamera = pCamera;
    };
    ~CheckFps(){};

public:
    double checkFps(const std::string &sn, const std::string &Info) {
        double fps      = 0.0;
        auto   t_cur    = std::chrono::steady_clock::now();
        auto   duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_cur - t_last).count();
        if (duration > m_duration) {
            fps        = frameCount / (duration / 1000);
            frameCount = 0;
            t_last     = t_cur;
            AINFO << "#camera[" << Info << "] SN[" << sn << "]'s FrameRate:" << fps << std::endl;
        }
        ++frameCount;
        return fps;
    }

private:
    AS_CAM_PTR                            m_pCamera;
    unsigned int                          frameCount = 0;
    std::chrono::steady_clock::time_point t_last;
    const unsigned int                    m_duration = 2000; /* ms */
};

class Camera {
public:
    Camera(AS_CAM_PTR pCamera, const AS_SDK_CAM_MODEL_E &cam_type);
    ~Camera();

public:
    int    init();
    void   Stop();
    void   Close();
    void   Restart();
    double checkFps();
    int    enableSaveImage(bool enable);
    int    enableDisplay(bool enable);
    bool   getDisplayStatus();
    int    getSerialNo(std::string &sn);
    int    getCameraAttrs(AS_CAM_ATTR_S &attr);
    void   saveImage(const AS_SDK_Data_s *pstData);
    void   saveMergeImage(const AS_SDK_MERGE_s *pstData);
    int    enableSaveImageToFile(bool enable);

    bool GetImageDatas(mstf::chss::proto::CameraPopDatas&);

    /**
     * @brief     convert the depth to clolor map for display
     * @param[out]color : color image
     * @param[in]depth : depth image
     * @param[in]max : depth image max value
     * @param[in]min : depth image min value
     * @return    0 success,non-zero error code.
     * @exception None
     * @author    Angstrong SDK develop Team
     * @date      2022/09/29
     */
    static void depth2color(cv::Mat &color, const cv::Mat &depth, const double max, const double min);

    cv::Mat yuyv2bgr(const cv::Mat &yuyv);

private:
    int  backgroundThread();
    void PublishAnImageRgb(const AS_Frame_s &img_data);
    void PublishAnImageDepth(const AS_Frame_s &img_data);
    void PublishAPointCloud(const AS_Frame_s &pc_data);

private:
    AS_CAM_PTR         m_handle = nullptr;
    std::string        m_serialno;
    CheckFps          *m_check_fps      = nullptr;
    bool               m_save_img       = false;
    bool               m_save_merge_img = false;
    /* display image by opecv show */
    bool               m_display        = false;
    bool               m_display_merge  = false;
    AS_CAM_ATTR_S      m_attr;
    AS_CAM_Parameter_s m_cam_parameter;
    AS_SDK_CAM_MODEL_E m_cam_type        = AS_SDK_CAM_MODEL_UNKNOWN;
    int                m_cnt             = 0; /* for kunlun a to save odd even */
    int                m_depthindex      = 0;
    int                m_rgbindex        = 0;
    int                m_pointCloudIndex = 0;
    int                m_irindex         = 0;
    int                m_peakindex       = 0;
    int                m_yuyvindex       = 0;
    bool               m_is_thread       = false;
    std::thread        m_backgroundThread;
    bool               callback_finished_{true};
    bool               has_image_{false};

    bool          save_to_file_{false};
    size_t        frame_seq_{0};
    std::string   frame_id_{"ascamera"};
    std::mutex    pub_mtx_;
    AS_SDK_Data_s sdk_data_;

    sensor_msgs::Image img_rgb_;
    sensor_msgs::Image img_depth_;
    sensor_msgs::PointCloud2 pcl_;
};

/*
 * @Date: 2023-11-15 21:00:26
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#pragma once
#include <chrono>
#include <thread>
#include <sstream>
#include <cstring>

#include "as_camera_sdk_api.h"
#include "common.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "cyber/common/log.h"
#include "modules/chassis/drivers/soc/soc_defs.h"

using namespace mstf;
using namespace chss;
using namespace driver;
using namespace apollo;

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
    double checkFps();
    int    enableSaveImage(bool enable);
    int    enableDisplay(bool enable);
    bool   getDisplayStatus();
    int    getSerialNo(std::string &sn);
    int    getCameraAttrs(AS_CAM_ATTR_S &attr);
    void   saveImage(const AS_SDK_Data_s *pstData);
    void   saveMergeImage(const AS_SDK_MERGE_s *pstData);
    void   displayImage(const std::string &serialno, const std::string &info, const AS_SDK_Data_s *pstData);
    void   displayMergeImage(const std::string &serialno, const std::string &info, const AS_SDK_MERGE_s *pstData);
    int    enableSaveImageToFile(bool enable);

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

    inline void SetSocListener(SocDataListener f)
    {
        soc_listener_ = f;
    }
private:
    int backgroundThread();

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

    bool save_to_file_{false};
    size_t frame_seq_{0};

    SocDataListener soc_listener_{nullptr};
};

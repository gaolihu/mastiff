/*
 * @Date: 2023-12-01 20:22:46
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera_srv.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#pragma once

#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <thread>

#include "as_camera_sdk_api.h"

class ICameraStatus {
public:
    virtual int  onCameraAttached(AS_CAM_PTR pCamera, const AS_SDK_CAM_MODEL_E &cam_type) = 0;
    virtual int  onCameraDetached(AS_CAM_PTR pCamera)                                     = 0;
    virtual int  onCameraOpen(AS_CAM_PTR pCamera)                                         = 0;
    virtual int  onCameraClose(AS_CAM_PTR pCamera)                                        = 0;
    virtual int  onCameraStart(AS_CAM_PTR pCamera)                                        = 0;
    virtual int  onCameraStop(AS_CAM_PTR pCamera)                                         = 0;
    virtual void onCameraNewFrame(AS_CAM_PTR pCamera, const AS_SDK_Data_s *pstData)       = 0;
    virtual void onCameraNewMergeFrame(AS_CAM_PTR pCamera, const AS_SDK_MERGE_s *pstData) = 0;
};

class CameraSrv {
public:
    CameraSrv(ICameraStatus *cameraStatus);
    ~CameraSrv();

    CameraSrv(const CameraSrv &)            = delete;
    CameraSrv &operator=(const CameraSrv &) = delete;

public:
    /**
     * @brief 打开、读取所有相机
     *
     * @return int
     */
    int Start();

    /**
     * @brief 关闭所有相机
     */
    void Stop();

    /**
     * @brief 关闭某个相机
     *
     * @param pCamera
     */
    void StopOne(AS_CAM_PTR pCamera);

    void Close();

    void Restart();

    /**
     * @brief 后台线程
     *
     * @return int
     */
    int StartMonitor();
    int StopMonitor();

    std::mutex &getLock() {
        return m_mutex;
    }

private:
    static void onNewFrame(AS_CAM_PTR pCamera, const AS_SDK_Data_s *pstData, void *privateData);
    static void onNewMergeFrame(AS_CAM_PTR pCamera, const AS_SDK_MERGE_s *pstData, void *privateData);
    int         getConfigFile(AS_CAM_PTR pCamera, std::string &configfile, AS_SDK_CAM_MODEL_E cam_type);
    int         scanDir(const std::string &dir, std::vector<std::string> &file);
    int         monitor();
    static void process(AS_CAM_PTR pCamera, void *privateData, float fProcess);

private:
    std::list<AS_CAM_PTR>      m_devsList;
    ICameraStatus             *m_camera_status;
    std::mutex                 m_mutex;
    bool                       m_monitor = false;
    std::thread                m_monitorThread;
    int                        m_timeout_count = 0;
    AS_SDK_CAM_MODEL_E         m_cam_type      = AS_SDK_CAM_MODEL_BUTT;
    std::map<AS_CAM_PTR, bool> m_streaming;
    bool                       have_started_{false};

    std::map<AS_CAM_PTR, std::chrono::high_resolution_clock::time_point> m_time;
};

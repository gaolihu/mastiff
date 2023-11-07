/*
 * @Date: 2023-10-17 20:13:12
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/angstrong_camera_node/as_camera_srv.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
/**
 * @file      CameraSrv.h
 * @brief     angstrong camera service header.
 *
 * Copyright (c) 2023 Angstrong Tech.Co.,Ltd
 *
 * @author    Angstrong SDK develop Team
 * @date      2023/05/12
 * @version   1.0

 */
#pragma once

#include <functional>
#include <list>
#include <thread>
#include <map>
#include <mutex>

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
    int         start();
    void        stop();
    int         startMonitor();
    int         stopMonitor();
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

    std::map<AS_CAM_PTR, std::chrono::high_resolution_clock::time_point> m_time;
};

/**
 * @file      CameraSrv.cpp
 * @brief     angstrong camera service.
 *
 * Copyright (c) 2022 Angstrong Tech.Co.,Ltd
 *
 * @author    Angstrong SDK develop Team
 * @date      2022/08/19
 * @version   1.0

 */

#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera_srv.h"

#include <dirent.h>
#include <errno.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

#include "common.h"
#include "cyber/common/log.h"

#define UPGRADE 0

CameraSrv::CameraSrv(ICameraStatus *cameraStatus) : m_camera_status(cameraStatus) {
    int ret = 0;
    AINFO << "Angstrong camera server" << std::endl;
    ret = AS_SDK_Init();
    if (ret != 0) {
        AFATAL << "sdk init failed" << std::endl;
    }
    char sdkVersion[64] = {0};
    ret                 = AS_SDK_GetSwVersion(sdkVersion, sizeof(sdkVersion));
    if (ret != 0) {
        AFATAL << "get sdk version failed" << std::endl;
    }
    AINFO << "Angstrong camera sdk version:" << sdkVersion << std::endl;
}

CameraSrv::~CameraSrv() {
    AINFO << "Angstrong camera server exit" << std::endl;
    int ret = AS_SDK_Deinit();
    if (ret != 0) {
        AFATAL << "sdk deinit failed" << std::endl;
    }
}

int CameraSrv::start() {
    int ret = -1;

    m_devsList.clear();
    ret = AS_SDK_GetCameraList(m_devsList);

    if (ret == -1) {
        AFATAL << "get camera list failed, ret " << ret << std::endl;
        return -1;
    }
    else {
        AINFO << "get camera cnt: " << m_devsList.size() << std::endl;
    }

    for (auto it = m_devsList.begin(); it != m_devsList.end(); it++) {
        AS_CAM_PTR pCamera = *it;

        // get model type
        ret = AS_SDK_GetCameraModel(pCamera, m_cam_type);
        AINFO << "get model type " << m_cam_type << std::endl;

        std::string file_path;
        if (getConfigFile(pCamera, file_path, m_cam_type) != 0) {
            AFATAL << "cannot find config file" << std::endl;
            return -1;
        }

        m_camera_status->onCameraAttached(pCamera, m_cam_type);

        // Open camera
        ret = AS_SDK_OpenCamera(pCamera, file_path.c_str());
        if (ret != 0) {
            AFATAL << "Open camera handle failed, confiPath " << file_path << std::endl;
            return -1;
        }
        m_camera_status->onCameraOpen(pCamera);

#if UPGRADE
        /* upgrade: before start stream */
        AS_CAM_Upgrade_Dev_s pstCallback;
        pstCallback.callback    = process;
        pstCallback.privateData = nullptr;
        char up_pkg[100]        = "../KUNLUN-C-L200C-OTA-ko-V01.03-app-V01.04_20230315.zip";
        AS_SDK_UpgradeDev(pCamera, up_pkg, &pstCallback);
        return 0;
#else
        // Register stream callback
        AS_CAM_Stream_Cb_s streamCallback;
        streamCallback.callback    = onNewFrame;
        streamCallback.privateData = this;

        ret = AS_SDK_RegisterStreamCallback(pCamera, &streamCallback);
        if (ret != 0) {
            AFATAL << "Register stream callback failed" << std::endl;
            return -1;
        }

        if (m_cam_type == AS_SDK_CAM_MODEL_KUNLUN_A) {
            // Register merge stream callback
            AS_CAM_Merge_Cb_s mergeStreamCallback;
            mergeStreamCallback.callback    = onNewMergeFrame;
            mergeStreamCallback.privateData = this;

            ret = AS_SDK_RegisterMergeFrameCallback(pCamera, &mergeStreamCallback);
            if (ret != 0) {
                AFATAL << "Register merge stream callback failed" << std::endl;
                return -1;
            }
        }

        // Start stream
        ret = AS_SDK_StartStream(pCamera);
        if (ret != 0) {
            AFATAL << "Start stream failed" << std::endl;
            return -1;
        }

        m_streaming.insert(std::make_pair(pCamera, true));
        m_time.insert(std::make_pair(pCamera, std::chrono::high_resolution_clock::now()));
#endif
    }
    return 0;
}

void CameraSrv::stop() {
    int ret     = 0;
    int dev_idx = 0;

    AINFO << "stop and close the camera" << std::endl;
    for (auto it = m_devsList.begin(); it != m_devsList.end(); it++) {
        AS_CAM_PTR dev = (AS_CAM_PTR)(*it);
        AINFO << "close camera idx " << dev_idx << std::endl;

        m_camera_status->onCameraStop(dev);
        ret = AS_SDK_StopStream(dev);
        if (ret < 0) {
            AFATAL << "stop stream failed, ret: " << ret << std::endl;
        }
        else {
            AINFO << "stop stream success" << std::endl;
        }

        m_camera_status->onCameraClose(dev);
        ret = AS_SDK_CloseCamera(dev);
        if (ret < 0) {
            AFATAL << "close camera failed, ret: " << ret << std::endl;
        }
        else {
            AINFO << "close camera success" << std::endl;
        }

        m_camera_status->onCameraDetached(dev);
        auto it_steam = m_streaming.find(dev);
        if (it_steam != m_streaming.end()) {
            it_steam->second = false;
            m_streaming.erase(dev);
        }

        auto it_time = m_time.find(dev);
        if (it_time != m_time.end()) {
            m_time.erase(dev);
        }

        dev_idx++;
    }

    ret = AS_SDK_FreeCameraList(m_devsList);
    AINFO << "Free Camera List, ret: " << ret << std::endl;
}

int CameraSrv::startMonitor() {
    m_monitor       = true;
    m_monitorThread = std::thread(&CameraSrv::monitor, this);
    // AINFO << "start to monitor." << std::endl;
    return 0;
}

int CameraSrv::stopMonitor() {
    if (m_monitor) {
        m_monitor = false;
        if (m_monitorThread.joinable()) {
            m_monitorThread.join();
        }
        AINFO << "stop monitor." << std::endl;
    }
    return 0;
}

void CameraSrv::onNewFrame(AS_CAM_PTR pCamera, const AS_SDK_Data_s *pstData, void *privateData) {
    CameraSrv *server = static_cast<CameraSrv *>(privateData);
    server->m_camera_status->onCameraNewFrame(pCamera, pstData);
    server->m_timeout_count = 0;

    for (auto it = server->m_devsList.begin(); it != server->m_devsList.end(); it++) {
        auto its = server->m_time.find(pCamera);
        if (its != server->m_time.end()) {
            its->second = std::chrono::high_resolution_clock::now();
            break;
        }
    }
}

void CameraSrv::onNewMergeFrame(AS_CAM_PTR pCamera, const AS_SDK_MERGE_s *pstData, void *privateData) {
    CameraSrv *server = static_cast<CameraSrv *>(privateData);
    server->m_camera_status->onCameraNewMergeFrame(pCamera, pstData);
    server->m_timeout_count = 0;

    for (auto it = server->m_devsList.begin(); it != server->m_devsList.end(); it++) {
        auto its = server->m_time.find(pCamera);
        if (its != server->m_time.end()) {
            its->second = std::chrono::high_resolution_clock::now();
            break;
        }
    }
}

int CameraSrv::getConfigFile(AS_CAM_PTR pCamera, std::string &configfile, AS_SDK_CAM_MODEL_E cam_type) {
    int         ret = 0;
    std::string name_key;
    switch (cam_type) {
    case AS_SDK_CAM_MODEL_KONDYOR:
    case AS_SDK_CAM_MODEL_KONDYOR_NET:
        name_key = "kondyor_";
        AINFO << "found a kondyor camera" << std::endl;
        break;
    case AS_SDK_CAM_MODEL_NUWA_XB40:
    case AS_SDK_CAM_MODEL_NUWA_X100:
    case AS_SDK_CAM_MODEL_NUWA_HP60:
    case AS_SDK_CAM_MODEL_NUWA_HP60V:
        name_key = "nuwa_";
        AINFO << "found a nuwa camera" << std::endl;
        break;
    case AS_SDK_CAM_MODEL_KUNLUN_A:
    case AS_SDK_CAM_MODEL_KUNLUN_C:
        name_key = "kunlun_";
        AINFO << "found a kunlun camera" << std::endl;
        break;
    case AS_SDK_CAM_MODEL_HP60C:
        name_key = "hp60c_";
        AINFO << "found a hp60c camera" << std::endl;
        break;
    case AS_SDK_CAM_MODEL_HP60CN:
        name_key = "hp60cn_";
        AINFO << "found a hp60cn camera" << std::endl;
        break;
    case AS_SDK_CAM_MODEL_VEGA:
        name_key = "vega_";
        AINFO << "found a vega camera" << std::endl;
        break;
    case AS_SDK_CAM_MODEL_CHANGA:
        name_key = "changA_";
        AINFO << "found a chang-A camera" << std::endl;
        break;
    default:
        AFATAL << "cam type error" << std::endl;
        return -1;
        break;
    }

    // get json
    std::vector<std::string> files;
    scanDir("./configurationfiles", files);
    ret = -1;
    for (auto it = files.begin(); it != files.end(); it++) {
        std::string filename = (*it).substr((*it).find_last_of("/"));
        if (filename.find(name_key) < filename.size()) {
            AINFO << "get file: " << *it << std::endl;
            configfile = (*it);
            ret        = 0;
            break;
        }
    }

    if (ret != 0) {
        AFATAL << "cannot find config file" << std::endl;
        return -1;
    }

    return 0;
}

int CameraSrv::scanDir(const std::string &dir, std::vector<std::string> &file) {
    int            ret = 0;
    DIR           *directory;
    struct dirent *ent;
    if (!(directory = opendir(dir.c_str()))) {
        std::cout << "can't not open dir:" << dir << std::endl;
        return -1;
    }
    while ((ent = readdir(directory)) != nullptr) {
        if (strncmp(ent->d_name, ".", 1) == 0) {
            continue;
        }
        if (ent->d_type == DT_REG) {
            std::string filepath(dir + "/" + ent->d_name);
            file.push_back(filepath);
        }
        if (ent->d_type == DT_DIR) {
            std::string childpath;
            childpath.append(dir);
            childpath.append("/");
            childpath.append(ent->d_name);
            scanDir(childpath, file);
        }
    }
    delete ent;
    closedir(directory);
    return ret;
}

void CameraSrv::process(AS_CAM_PTR pCamera, void *privateData, float fProcess) {
    AINFO << " upgrade process:" << fProcess << std::endl;
}

int CameraSrv::monitor() {
    auto st_former = std::chrono::high_resolution_clock::now();
    auto st_later  = std::chrono::high_resolution_clock::now();
    auto duration  = std::chrono::duration_cast<std::chrono::seconds>(st_later - st_former).count();
    bool restart   = false;

    // check stream is ready
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    while (m_monitor) {
        for (auto it = m_devsList.begin(); it != m_devsList.end(); it++) {
            AS_CAM_PTR pCamera  = *it;
            auto       its      = m_streaming.find(pCamera);
            auto       its_time = m_time.find(pCamera);
            if (its != m_streaming.end()) {
                if (its->second == true) {
                    st_later  = std::chrono::high_resolution_clock::now();
                    st_former = its_time->second;
                    duration  = std::chrono::duration_cast<std::chrono::seconds>(st_later - st_former).count();
                    // AINFO << "############ duration " << duration << std::endl;
                    if (duration < -2 || duration > 2) {
                        restart = true;
                        break;
                    }
                }
            }
        }

        if (restart) {
            AINFO << "detect device expection, try to reconnect..." << std::endl;
            restart = false;
            stop();
            start();
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    return 0;
}

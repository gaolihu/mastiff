#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera_ctrl.h"

#include <dirent.h>
#include <errno.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

#include "as_camera_sdk_api.h"
#include "as_camera_sdk_def.h"
#include "common.h"

#ifdef CFG_X11_ON
#include <X11/Xlib.h>
#endif

namespace mstf {
namespace chss {
namespace driver {


//SocDataListener AsCameraCtrl::soc_listener_ = nullptr;

AsCameraCtrl::AsCameraCtrl() {
#ifdef CFG_X11_ON
    XInitThreads();
#endif
}

AsCameraCtrl::~AsCameraCtrl() {}

bool AsCameraCtrl::SetCameraCtrl(const proto::CameraSetting &ctrl) {
    if (ctrl.close()) {
        server->Close();
        return true;
    }
    else if (ctrl.restart()) {
        server->Restart();
        return true;
    }

    for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
        if (ctrl.capture()) {
            it->second->enableSaveImageToFile(true);
        }

        if (ctrl.video()) {}
    }

    return true;
}

int AsCameraCtrl::Start() {
    int ret = 0;
    if (server == nullptr) {
        server = new CameraSrv(this);
        ret    = server->Start();
        if (ret != 0) {
            AFATAL << "start server failed";
        }
        server->StartMonitor();
    }

    return ret;
}

void AsCameraCtrl::Stop() {
    /* stop streaming and close the camera */
    if (server != nullptr) {
        server->Stop();
        server->StopMonitor();
        /* it will stop the hotplug monitor */
        delete server;
        server = nullptr;
    }

    /* free the map */
    m_camera_map.erase(m_camera_map.begin(), m_camera_map.end());
}

void AsCameraCtrl::Display(bool enable) {
    for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
        it->second->enableDisplay(enable);
    }
}

bool AsCameraCtrl::GetDisplayStatus() {
    bool status = false;
    for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
        status = it->second->getDisplayStatus();
    }
    return status;
}

void AsCameraCtrl::SaveImage() {
    for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
        it->second->enableSaveImage(true);
    }
}

void AsCameraCtrl::LogFps(bool enable) {
    m_logfps = enable;
}

bool AsCameraCtrl::GetLogFps() {
    return m_logfps;
}
//void AsCameraCtrl::CameraMsgCallback(const Message &msg) {
    //if (soc_listener_) {
        //soc_listener_(msg);
    //}
    //else {
        //AERROR << "soc publisher not set, can not send msg";
    //}
    //TODO GLH, 2024/1/27
//}
int AsCameraCtrl::onCameraAttached(AS_CAM_PTR pCamera, const AS_SDK_CAM_MODEL_E &cam_type) {
    auto c = std::make_shared<Camera>(pCamera, cam_type);
    //c->SetMsgPublisher(std::bind(&AsCameraCtrl::CameraMsgCallback, ::_1));
    //TODO glh
    m_camera_map.insert({pCamera, c});
    AINFO << "camera attached, now camera count: " << m_camera_map.size();

    return 0;
}

int AsCameraCtrl::onCameraDetached(AS_CAM_PTR pCamera) {
    auto camIt = m_camera_map.find(pCamera);
    if (camIt != m_camera_map.end()) {
        camIt->second->Stop();
        m_camera_map.erase(pCamera);
        AINFO << "camera detached, left camera count: " << m_camera_map.size();
    }

    return 0;
}

int AsCameraCtrl::onCameraOpen(AS_CAM_PTR pCamera) {
    AINFO << "camera opened";
    auto camIt = m_camera_map.find(pCamera);
    if (camIt != m_camera_map.end()) {
        camIt->second->init();
    }
    return 0;
}

int AsCameraCtrl::onCameraClose(AS_CAM_PTR pCamera) {
    onCameraDetached(pCamera);
    AINFO << "camera closed";
    return 0;
}

int AsCameraCtrl::onCameraStart(AS_CAM_PTR pCamera) {
    AINFO << "camera start streaming";
    return 0;
}

int AsCameraCtrl::onCameraStop(AS_CAM_PTR pCamera) {
    onCameraDetached(pCamera);
    AINFO << "camera stop streaming";
    return 0;
}

void AsCameraCtrl::onCameraNewFrame(AS_CAM_PTR pCamera, const AS_SDK_Data_s *pstData) {
    AS_CAM_ATTR_S attr;

    auto camIt = m_camera_map.find(pCamera);

    if (camIt != m_camera_map.end()) {
        if (m_logfps) {
            camIt->second->checkFps();
        }
        camIt->second->enableSaveImage(true);
        camIt->second->saveImage(pstData);
    }
}

void AsCameraCtrl::onCameraNewMergeFrame(AS_CAM_PTR pCamera, const AS_SDK_MERGE_s *pstData) {
    AS_CAM_ATTR_S attr;

    auto camIt = m_camera_map.find(pCamera);
    if (camIt != m_camera_map.end()) {
        if (m_logfps) {
            camIt->second->checkFps();
        }
        camIt->second->saveMergeImage(pstData);
    }
}

void AsCameraCtrl::LogCfgParameter() {
    for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
        AS_SDK_LogCameraCfg(it->first);
    }
}

bool AsCameraCtrl::virtualMachine() {
    int   cnt = 0;
    char  szCnt[8];
    FILE *fp = nullptr;

    char cmd[128];
    snprintf(cmd, sizeof(cmd) - 1, R"(lscpu | grep "Hypervisor vendor" | wc -l)");
    fp = popen(cmd, "r");
    if (fgets(szCnt, sizeof(szCnt), fp) != nullptr) {
        if (strlen(szCnt) != 0) {
            cnt = std::stoi(szCnt);
        }
    }
    pclose(fp);
    fp = nullptr;
    if (cnt == 0) {
        return false;
    }
    else {
        return true;
    }
}

}  // namespace driver
}  // namespace chss
}  // namespace mstf

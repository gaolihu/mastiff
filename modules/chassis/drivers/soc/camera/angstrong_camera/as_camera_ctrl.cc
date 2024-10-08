#include "as_camera_sdk_api.h"
#include "as_camera_sdk_def.h"

#include "modules/chassis/drivers/soc/camera/angstrong_camera/as_camera_ctrl.h"

namespace mstf {
namespace chss {
namespace driver {

    AsCameraCtrl::AsCameraCtrl(const std::string& dev) {
        AINFO << "Camera ctrl " << dev;
        dev_ = dev;
    }

    AsCameraCtrl::~AsCameraCtrl() {}

    int AsCameraCtrl::SetCameraCtrl(const CameraSetting &ctrl) {
        if (ctrl.close()) {
            server->Close();
            return 0;
        } else if (ctrl.restart()) {
            server->Restart();
            return 0;
        }

        for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
            if (ctrl.capture()) {
                it->second->enableSaveImageToFile(true);
            }

            if (ctrl.video()) {}
        }

        return 0;
    }

    int AsCameraCtrl::SetCameraCtrl(const
            RepeatedPtrField<CameraSetting>& ctrl) {
        for (auto c : ctrl) {
            SetCameraCtrl(c);
        }
        return 0;
    }

    int AsCameraCtrl::Init() {
        AINFO << "init angstrong camera: " << dev_;
        return 0;
    }

    int AsCameraCtrl::Start() {
        AINFO << "start angstrong camera: " << dev_;
        int ret = 0;
        if (server == nullptr) {
            server = new CameraSrv(this);
            ret = server->Start();
            if (ret != 0) {
                AFATAL << "start server failed";
            }
            server->StartMonitor();
        }

        return ret;
    }

    int AsCameraCtrl::Stop() {
        /* stop streaming and stop the camera */
        if (server != nullptr) {
            server->StopMonitor();
            /* it will stop the hotplug monitor */
        }
        return 0;
    }

    int AsCameraCtrl::Resume() {
        return 0;
    }

    int AsCameraCtrl::Close() {
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
        return 0;
    }

    int AsCameraCtrl::PollingCameraRutine(CameraPopDatas& data) {
        if (m_camera_map.size() == 0)
            //no camera
            return false;

        camera_it_++;
        if (camera_it_ == m_camera_map.end()) {
            camera_it_ = m_camera_map.begin();
        }

        data.set_camera_id(std::distance(camera_it_, m_camera_map.begin()));
        return camera_it_->second->GetImageDatas(data);
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

    //TODO
    int AsCameraCtrl::SaveImage(const std::string& st) {
        for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
            it->second->enableSaveImage(true);
        }
        return 0;
    }

    void AsCameraCtrl::LogFps(bool enable) {
        m_logfps = enable;
    }

    bool AsCameraCtrl::GetLogFps() {
        return m_logfps;
    }

    int AsCameraCtrl::onCameraAttached(AS_CAM_PTR pCamera, const AS_SDK_CAM_MODEL_E &cam_type) {
        auto c = std::make_shared<Camera>(pCamera, cam_type);
        m_camera_map.insert({pCamera, c});
        camera_it_ = m_camera_map.begin();
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

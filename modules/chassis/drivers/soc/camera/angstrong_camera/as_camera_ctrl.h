#pragma once

#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <thread>
#include <unordered_map>

#include "as_camera_sdk_api.h"
#include "as_camera_sdk_def.h"

#include "modules/chassis/drivers/soc/camera/angstrong_camera/as_camera.h"
#include "modules/chassis/drivers/soc/camera/angstrong_camera/as_camera_srv.h"

#include "modules/chassis/drivers/soc/camera/camera_itf.h"

namespace mstf {
namespace chss {
namespace driver {

    class AsCameraCtrl :
        public CameraItf,
        public ICameraStatus {
            public:
                AsCameraCtrl(const std::string&);
                ~AsCameraCtrl();

            public:
                virtual int Init() override;
                virtual int Start() override;
                virtual int Stop() override;
                virtual int Resume() override;
                virtual int Close() override;

                virtual int PollingCameraRutine(CameraPopDatas&) override;
                virtual int SaveImage(const std::string&) override;

                virtual int SetCameraCtrl(const CameraSetting&) override;
                virtual int SetCameraCtrl(const
                        RepeatedPtrField<CameraSetting>&) override;

            private:
                void        Display(bool enable);
                bool        GetDisplayStatus();
                void        LogFps(bool enable);
                bool        GetLogFps();
                void        LogCfgParameter();

                virtual int  onCameraAttached(AS_CAM_PTR pCamera, const AS_SDK_CAM_MODEL_E &cam_type) override;
                virtual int  onCameraDetached(AS_CAM_PTR pCamera) override;
                virtual int  onCameraOpen(AS_CAM_PTR pCamera) override;
                virtual int  onCameraClose(AS_CAM_PTR pCamera) override;
                virtual int  onCameraStart(AS_CAM_PTR pCamera) override;
                virtual int  onCameraStop(AS_CAM_PTR pCamera) override;
                virtual void onCameraNewFrame(AS_CAM_PTR pCamera, const AS_SDK_Data_s *pstData) override;
                virtual void onCameraNewMergeFrame(AS_CAM_PTR pCamera, const AS_SDK_MERGE_s *pstData) override;
                bool         virtualMachine();

            private:
                std::string dev_;
                CameraSrv *server   = nullptr;
                /* log the average frame rate */
                bool       m_logfps = false;

                std::unordered_map<AS_CAM_PTR, std::shared_ptr<Camera>> m_camera_map;
                std::unordered_map<AS_CAM_PTR, std::shared_ptr<Camera>>::iterator camera_it_;
        };
}  // namespace driver
}  // namespace chss
}  // namespace mstf

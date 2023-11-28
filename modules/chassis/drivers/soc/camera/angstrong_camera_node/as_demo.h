/*
 * @Date: 2023-10-17 20:14:02
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/camera/angstrong_camera_node/as_demo.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
#pragma once

#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "as_camera_sdk_api.h"
#include "as_camera_sdk_def.h"
#include "cyber/common/log.h"
#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera.h"
#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera_srv.h"
#include "modules/chassis/drivers/soc/soc_defs.h"
#include "modules/chassis/proto/frame_down_stream.pb.h"
#include "modules/chassis/proto/frame_up_stream.pb.h"

namespace mstf {
namespace chss {
namespace camera {
using namespace driver;
class Demo : public ICameraStatus {
public:
    Demo();
    ~Demo();

public:
    int  start();
    void stop();
    void saveImage();
    void display(bool enable);
    bool getDisplayStatus();
    void logFps(bool enable);
    bool getLogFps();
    void logCfgParameter();

    bool SetCameraCtrl(const proto::CameraCtrl &ctrl);
    void SetSocListener(SocDataListener f) {
        for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
            it->second->SetSocListener(f);
        }
        soc_listener_ = f;
    }

private:
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
    CameraSrv *server   = nullptr;
    /* log the average frame rate */
    bool       m_logfps = false;

    std::unordered_map<AS_CAM_PTR, Camera *> m_camera_map;

    SocDataListener soc_listener_{nullptr};
};
}  // namespace camera
}  // namespace chss
}  // namespace mstf

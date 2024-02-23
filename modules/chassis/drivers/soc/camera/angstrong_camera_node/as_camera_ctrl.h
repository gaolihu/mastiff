#pragma once

#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <thread>
#include <unordered_map>

#include "as_camera_sdk_api.h"
#include "as_camera_sdk_def.h"
#include "cyber/common/log.h"
#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera.h"
#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera_srv.h"
#include "modules/chassis/proto/chss_io.pb.h"

namespace mstf {
namespace chss {
namespace driver {

using namespace proto;

class AsCameraCtrl : public ICameraStatus {
public:
    AsCameraCtrl(const std::string&);
    ~AsCameraCtrl();

public:
    int         Start();
    void        Stop();
    void        Close();
    void        SaveImage();
    void        Display(bool enable);
    bool        GetDisplayStatus();
    void        LogFps(bool enable);
    bool        GetLogFps();
    void        LogCfgParameter();

    bool SetCameraCtrl(const proto::CameraSetting &ctrl);
    bool SetCameraCtrl(const google::protobuf::RepeatedPtrField<proto::CameraSetting> &ctrl);

    bool PollingCameraRutine(CameraPopDatas&);

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

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

using namespace std::placeholders;

namespace mstf {
namespace chss {
namespace driver {

class AsCameraCtrl : public ICameraStatus {
public:
    AsCameraCtrl();
    ~AsCameraCtrl();

public:
    int         Start();
    void        Stop();
    void        SaveImage();
    void        Display(bool enable);
    bool        GetDisplayStatus();
    void        LogFps(bool enable);
    bool        GetLogFps();
    void        LogCfgParameter();
    /**
     * @brief Camera里读取到数据后调用这个回调发送msg
     *
     * @param msg
     */
    //static void CameraMsgCallback(const Message &msg);

    /**
     * @brief Set the Camera Ctrl object, 按键控制相机动作
     *
     * @param ctrl
     * @return true
     * @return false
     */
    bool SetCameraCtrl(const proto::CameraSetting &ctrl);

    /**
     * @brief 从 SocData 注册过来的回调函数
     *
     * @param f
     */
    /*
    void SetSocListener(SocDataListener f) {
        if (!f) {
            AERROR << "set soc data listener but empty";
            return;
        }

        soc_listener_ = f;
        for (auto it = m_camera_map.begin(); it != m_camera_map.end(); it++) {
            it->second->SetMsgPublisher(soc_listener_);
        }
    }
    */

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

    std::unordered_map<AS_CAM_PTR, std::shared_ptr<Camera>> m_camera_map;

    //static SocDataListener soc_listener_;
};
}  // namespace driver
}  // namespace chss
}  // namespace mstf

#include "modules/chassis/parser/common_inf.h"
#include "modules/chassis/drivers/soc/soc_data.h"

namespace mstf {
namespace chss {
namespace driver {

    SocData::SocData(const SocDataListener& soc_l,
            const std::shared_ptr<ChassisConfig>& cc) {
#ifdef CHSS_PKG_DBG
        AINFO << "SocData construct";
#endif
        soc_listner_ = soc_l;

        chs_conf_ = cc;
        DriveDataItf::Init("soc drive", -1);
    }

    SocData::~SocData() {
#ifdef CHSS_PKG_DBG
        AINFO << "SocData de-construct, dev: " << dev_;
#endif
    }

    int SocData::Init(const SensorIndicator* si) {
        AINFO << "init soc dev driver:\n" <<
            si->DebugString();

        if (si->dev_main() != EE_DEV_MAIN_SOC) {
            AERROR << "init soc device error";
            return -1;
        }

        if (si->type() == E_DEVICE_JOYSTICK) {
            js_ = std::make_shared<JoyStickData>
                (parser::CommonItf::Instance()->GetDevJsConf(si));
            js_->Init();
        }

        if(si->type() == E_DEVICE_AUDIO) {
            audio_ctrl_ = std::make_shared<AudioCtrl>(
                    parser::CommonItf::Instance()->GetDevAudConf(si));
            audio_ctrl_->Init();
        }

        if(si->type() == E_DEVICE_WIRELESS) {
            wifi_ctrl_ = std::make_shared<WiFiThread>();
            wifi_ctrl_->Init();
        }

        if(si->type() == E_DEVICE_CAMERA) {
            camera_ctrl_ = std::make_shared<AsCameraCtrl>();
            //camera_ctrl_->Init();
        }

        return 0;
    }

    int SocData::Start(const SensorIndicator* si) {
        AINFO << "soc data start\n" <<
            si->DebugString();

        DriveDataItf::Start();

        if (js_ && si->type() == E_DEVICE_JOYSTICK) {
            js_->Start();
        }

        if (audio_ctrl_ && si->type() == E_DEVICE_AUDIO) {
            audio_ctrl_->Start();
        }

        if(wifi_ctrl_ && si->type() == E_DEVICE_WIRELESS) {
            wifi_ctrl_->Start();
        }

        if(camera_ctrl_ && si->type() == E_DEVICE_WIRELESS) {
            camera_ctrl_->Start();
        }

        return 0;
    }

    int SocData::Stop(const SensorIndicator* si) {
        AINFO << "soc data stop\n" <<
            si->DebugString();

        if (js_ && si->type() == E_DEVICE_JOYSTICK) {
            js_->Stop();
        }

        if (audio_ctrl_ && si->type() == E_DEVICE_AUDIO) {
            audio_ctrl_->Stop();
        }

        if(wifi_ctrl_ && si->type() == E_DEVICE_WIRELESS) {
            wifi_ctrl_->Stop();
        }

        if(camera_ctrl_ && si->type() == E_DEVICE_WIRELESS) {
            camera_ctrl_->Stop();
        }

        return 0;
    }

    int SocData::Close(const SensorIndicator* si) {
        AINFO << "soc data close\n" <<
            si->DebugString();

        if (js_ && si->type() == E_DEVICE_JOYSTICK) {
            js_->Close();
        }

        if (audio_ctrl_ && si->type() == E_DEVICE_AUDIO) {
            audio_ctrl_->Close();
        }

        if(wifi_ctrl_ && si->type() == E_DEVICE_WIRELESS) {
            wifi_ctrl_->Close();
        }

        if(camera_ctrl_ && si->type() == E_DEVICE_WIRELESS) {
            //camera_ctrl_->Close();
        }

        return 0;
    }

    int SocData::Resume(const SensorIndicator* si) {
        AINFO << "soc data resume\n" <<
            si->DebugString();

        if (js_ && si->type() == E_DEVICE_JOYSTICK) {
            //js_->Resume();
        }

        if (audio_ctrl_ && si->type() == E_DEVICE_AUDIO) {
            //audio_ctrl_->Resume();
        }

        if(wifi_ctrl_ && si->type() == E_DEVICE_WIRELESS) {
            //wifi_ctrl_->Resume();
        }

        if(camera_ctrl_ && si->type() == E_DEVICE_WIRELESS) {
            //camera_ctrl_->Resume();
        }

        return 0;
    }

    int SocData::SocWrite(const Message& msg) {
        #ifdef CHSS_PKG_DBG
        AINFO << "write soc:\n" << msg.DebugString();
        #endif

#if 0
        const DownToSocData data =
            dynamic_cast<const DownToSocData&>(msg);

        if (data.has_gpio()) {
            //TODO 1,
        }

        if (data.has_pwm()) {
            //TODO 2,
        }

        if (data.has_audio()) {
            //TODO 3,
            //ChassisConfig AudioDevice AudioConfig
            audio_ctrl_->SetAudioCtrl(data.audio());
        }

        if (data.has_camera()) {
            /*bool ret = */camera_ctrl_->SetCameraCtrl(data.camera());
            //TODO: if ret=false, do something?
        }

        if(data.has_wireless()){
            // call wifi tool
            wifi_ctrl_->SetWiFiCtrl(data.wireless());
        }

#endif
        return 0;
    }

    void SocData::PollingDriveRutine() {
        if (soc_listner_ == nullptr)
            AFATAL << "no soc listener";

        if (js_) {
            std::vector<uint8_t> data;
            js_->PollingJsRutine(data);
            if (!data.empty()) {
                AINFO << "has data!";
            }
        }

        //audio TODO
        //network TODO
        //camera TODO
        //GLH, 2024/1/27, 16:57

        return;
    }

} //namespace driver
} //namespace chss
} //namespace mstf

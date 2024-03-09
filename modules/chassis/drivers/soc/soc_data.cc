#include "modules/chassis/parser/common_inf.h"
#include "modules/chassis/drivers/soc/soc_data.h"

namespace mstf {
namespace chss {
namespace driver {

    SocData::SocData(const SocDataListener& soc_l,
            const std::shared_ptr<ChassisConfig>& cc) {
#ifdef CHSS_PKG_DBG
        AINFO << "SOC SocData drive construct";
#endif
        soc_listner_ = soc_l;

        //chs_conf_ = cc;
        DriveDataItf::Init("SOC-Drive", -1);
        DriveDataItf::Start();
    }

    SocData::~SocData() {
#ifdef CHSS_PKG_DBG
        AINFO << "SocData de-construct";
#endif
    }

    int SocData::Init(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "init soc dev driver:\n" <<
            si->DebugString();
#endif
        if (si->dev_main() != EE_DEV_MAIN_SOC) {
            AERROR << "init soc device error";
            return -1;
        }

        if (si->type() == E_DEVICE_JOYSTICK) {
            js_.first = si;
            js_.second = std::make_shared<JoyStickData>
                (parser::CommonItf::Instance()->GetDevJsConf(si));
            js_.second->Init();
        }

        if(si->type() == E_DEVICE_AUDIO) {
            audio_ctrl_.first = si;
            audio_ctrl_.second = std::make_shared<AudioCtrl>
                (parser::CommonItf::Instance()->GetDevAudConf(si));
            audio_ctrl_.second->Init();
        }

        if(si->type() == E_DEVICE_WIRELESS) {
            wifi_ctrl_.first = si;
            wifi_ctrl_.second = std::make_shared<WiFiThread>();
            wifi_ctrl_.second->Init();
        }

        if(si->type() == E_DEVICE_CAMERA) {
            camera_itf_.first = si;

            if (si->ihi().type().find("Angstrong") != std::string::npos) {
                camera_itf_.second =
                    std::make_shared<AsCameraCtrl>(si->ihi().name());
                camera_itf_.second->Init();
            } else if (si->ihi().type().find("Oak") != std::string::npos) {
                camera_itf_.second =
                    std::make_shared<LuxonisCamera>(si->ihi().name());
                camera_itf_.second->Init();
            } else {
                AWARN << "no supported cameras!";
            }
        }

        return 0;
    }

    int SocData::Start(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "soc data start\n" <<
            si->DebugString();
#endif
        if (js_.first && si->type() == E_DEVICE_JOYSTICK) {
            js_.second->Start();
            js_working_ = true;
        }

        if (audio_ctrl_.first && si->type() == E_DEVICE_AUDIO) {
            audio_ctrl_.second->Start();
            audio_working_ = true;
        }

        if(wifi_ctrl_.first && si->type() == E_DEVICE_WIRELESS) {
            wifi_ctrl_.second->Start();
            wifi_working_ = true;
        }

        if(camera_itf_.first && si->type() == E_DEVICE_CAMERA) {
            camera_itf_.second->Start();
            camera_working_ = true;
        }

        return 0;
    }

    int SocData::Stop(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "soc data stop\n" <<
            si->DebugString();
#endif
        if (js_.first && si->type() == E_DEVICE_JOYSTICK) {
            js_.second->Stop();
            js_working_ = false;
        }

        if (audio_ctrl_.first && si->type() == E_DEVICE_AUDIO) {
            audio_ctrl_.second->Stop();
            audio_working_ = false;
        }

        if(wifi_ctrl_.first && si->type() == E_DEVICE_WIRELESS) {
            wifi_ctrl_.second->Stop();
            wifi_working_ = false;
        }

        if(camera_itf_.first && si->type() == E_DEVICE_CAMERA) {
            camera_itf_.second->Stop();
            camera_working_ = false;
        }

        return 0;
    }

    int SocData::Close(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "soc data close\n" <<
            si->DebugString();
#endif
        if (js_.first && si->type() == E_DEVICE_JOYSTICK) {
            js_.second->Close();
            js_working_ = false;
        }

        if (audio_ctrl_.first && si->type() == E_DEVICE_AUDIO) {
            audio_ctrl_.second->Close();
            audio_working_ = false;
        }

        if(wifi_ctrl_.first && si->type() == E_DEVICE_WIRELESS) {
            wifi_ctrl_.second->Close();
            wifi_working_ = false;
        }

        if(camera_itf_.first && si->type() == E_DEVICE_CAMERA) {
            camera_itf_.second->Close();
            camera_working_ = false;
        }

        return 0;
    }

    int SocData::Resume(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "soc data resume\n" <<
            si->DebugString();
#endif
        if (js_.first && si->type() == E_DEVICE_JOYSTICK) {
            //js_.second->Resume();
        }

        if (audio_ctrl_.first && si->type() == E_DEVICE_AUDIO) {
            //audio_ctrl_.second->Resume();
        }

        if(wifi_ctrl_.first && si->type() == E_DEVICE_WIRELESS) {
            //wifi_ctrl_.second->Resume();
        }

        if(camera_itf_.first && si->type() == E_DEVICE_CAMERA) {
            //camera_itf_.second->Resume();
        }

        return 0;
    }

    int SocData::SocWrite(const SensorIndicator* si,
            const Message& msg) {
#ifdef CHSS_PKG_DBG
        AINFO << "write soc:\n" << si->DebugString() <<
            "msg to soc:\n" << msg.DebugString();
#endif
        if (audio_ctrl_.first && si->type() == E_DEVICE_AUDIO) {
            //audio play
            audio_ctrl_.second->SetAudioCtrl(dynamic_cast<const
                    ChsSocMiscCtrl&>(msg).audio());
        }

        if (wifi_ctrl_.first && si->type() == E_DEVICE_WIRELESS) {
            // call wifi tool
            wifi_ctrl_.second->SetWiFiCtrl(dynamic_cast<const
                    ChsSocMiscCtrl&>(msg).wifi());
        }

        if (camera_itf_.first && si->type() == E_DEVICE_CAMERA) {
            camera_itf_.second->SetCameraCtrl(dynamic_cast<const
                    ChsSocMiscCtrl&>(msg).camera());
        }

#if 0
        if (data.has_gpio()) {
            //TODO 1,
        }

        if (data.has_pwm()) {
            //TODO 2,
        }
#endif
        return 0;
    }

    void SocData::PollingDriveRutine() {
        if (js_.first && js_working_) {
            //joystick TODO
            std::vector<uint8_t> data;
            js_.second->PollingJsRutine(data);
            if (!data.empty()) {
                AINFO << "has data!";
                NullMsg n;
                soc_listner_(js_.first, n, &data[0], data.size());
            }
        }

        //audio TODO
        if (audio_ctrl_.first && audio_working_) {
            audio_ctrl_.second->PollingAudioRutine();
            //soc_listner_(audio_ctrl_.first, );
        }

        //network TODO
        if (wifi_ctrl_.first && wifi_working_) {
            WifiInfo wi;
            wifi_ctrl_.second->PollingNetworkRutine(wi);
            soc_listner_(wifi_ctrl_.first, wi, nullptr, 0);
        }

        //camera TODO
        if (camera_itf_.first && camera_working_) {
            CameraPopDatas cam;
            if (camera_itf_.second->PollingCameraRutine(cam))
                soc_listner_(camera_itf_.first, cam, nullptr, 0);
        }


        return;
    }

} //namespace driver
} //namespace chss
} //namespace mstf

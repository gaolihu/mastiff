#include "modules/chassis/drivers/soc/soc_data.h"

#include "modules/chassis/proto/frame_up_stream.pb.h"

namespace mstf {
namespace chss {
namespace driver {

    SocData::SocData(const SocDataListener& listner) {
#ifdef CHSS_PKG_DBG
        AINFO << "SocData construct";
#endif
        soc_listner_ = listner;
    }

    SocData::~SocData() {
#ifdef CHSS_PKG_DBG
        AINFO << "SocData de-construct, dev: " << dev_;
#endif
    }

    int SocData::Init(const SensorInfo* si,
            const ChassisConfig* cc) {
        if (chs_conf_ != nullptr) {
            AINFO << "soc init config done!";
            chs_conf_ = cc;
        } else {
            //AWARN << "soc init what!";
        }

        if (si != 0) {
            AINFO << "init soc dev driver:\n" <<
                si->DebugString();
            //TODO
        }

        return 0;
    }

    int SocData::Start(const SensorInfo* si) {
        AINFO << "soc data start\n" <<
            si->DebugString();

        // TODO, use proto
        if (si->dev_type() == E_DEVICE_CAMERA) {
            camera_ctrl_.start();
        } else {
            //TODO
        }

        return 0;
    }

    int SocData::Stop(const SensorInfo* si) {
        AINFO << "soc data stop\n" <<
            si->DebugString();

        if (si->dev_type() == E_DEVICE_CAMERA) {
            camera_ctrl_.stop();
        }
        return 0;
    }

    int SocData::Close(const SensorInfo* si) {
        AINFO << "soc data close\n" <<
            si->DebugString();

        return 0;
    }

    int SocData::SocWrite(const Message& msg) {
        #ifdef CHSS_PKG_DBG
        AINFO << "write soc:\n" << msg.DebugString();
        #endif

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
        }

        if (data.has_camera()) {
            if(soc_listner_){
                camera_ctrl_.SetSocListener(soc_listner_);
            }
            /*bool ret = */camera_ctrl_.CameraOperate(data.camera());
            //TODO: if ret=false, do something?
        }

        if(data.has_wireless()){
            // call wifi tool
            auto result = wifi_tool_.WifiOperate(data.wireless());
            auto response = mstf::chss::proto::WifiResponse();
            if(soc_listner_ && !result.empty()){
                response.set_wifi_result(result);
                soc_listner_(response);
            }
        }

        return 0;
    }

} //namespace driver
} //namespace chss
} //namespace mstf

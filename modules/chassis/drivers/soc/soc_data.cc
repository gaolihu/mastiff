#include "modules/chassis/drivers/soc/soc_data.h"

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

    int SocData::Init(const ChassisConfig* c) {
        AINFO << "soc data init!";
        chs_conf_ = c;
        return 0;
    }

    int SocData::Start(/* TODO */) {
        // TODO, use proto
        AINFO << "soc data start!";
        return 0;
    }

    int SocData::Stop(EE_DEVICE_TYPE dev_type, const std::string dev) {
        AINFO << "soc data stop!";
        return 0;
    }

    int SocData::Close(EE_DEVICE_TYPE dev_type, const std::string dev) {
        AINFO << "soc data close!";
        return 0;
    }

    int SocData::SocWrite(const Message& msg) {
        AINFO << "write soc:\n" << msg.DebugString();

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
            //TODO 4,   
        }

        return 0;
    }

} //namespace driver
} //namespace chss
} //namespace mstf

#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/audio_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/audio_play.h"

namespace mstf {
namespace chss {
namespace device {

    AudioPlay::AudioPlay(const
            SensorIndicator& idc) :
        DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioPlay construct";
#endif
        data_parser_ = std::make_unique
            <AudioParser>(idc);
        /*
         * TODO
        data_packer_ = std::make_unique
            <AudioParser>(idc);
            */

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    AudioPlay::~AudioPlay() {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioPlay de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

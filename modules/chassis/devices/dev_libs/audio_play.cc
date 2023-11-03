#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/audio_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/audio_play.h"

namespace mstf {
namespace chss {
namespace device {

    AudioPlay::AudioPlay(const ChassisConfig* cc) :
            DeviceBaseItf(cc->aud_dev().si(),
                    cc->aud_dev().sn_ind()) {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioPlay construct" <<
#if 0
            ":\n{\n" <<
            cc->aud_dev().DebugString() << "}";
#else
            "";
#endif
#endif
        data_parser_ = std::make_unique
            <AudioParser>(cc, &cc->aud_dev().si());
        DataTransact::Instance()->RegisterDevice(
                cc->aud_dev().si().name(),
                cc->aud_dev().sn_ind(),
                dynamic_cast<DeviceBaseItf*>(this));
    }

    AudioPlay::~AudioPlay() {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioPlay de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

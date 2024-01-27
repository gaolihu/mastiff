#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class AudioPlay : public DeviceBaseItf {
        public:
            AudioPlay(const SensorIndicator&);
            virtual ~AudioPlay();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

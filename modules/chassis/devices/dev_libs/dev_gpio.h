#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceGpio : public DeviceBaseItf {
        public:
            DeviceGpio(const SensorIndicator&);
            virtual ~DeviceGpio();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

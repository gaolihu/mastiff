#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceJoyStick : public DeviceBaseItf {
        public:
            DeviceJoyStick(const SensorIndicator&);
            virtual ~DeviceJoyStick();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

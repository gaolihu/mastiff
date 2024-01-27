#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceIMU : public DeviceBaseItf {
        public:
            DeviceIMU(const SensorIndicator&);
            virtual ~DeviceIMU();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

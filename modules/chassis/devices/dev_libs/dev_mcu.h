#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceMcu : public DeviceBaseItf {
        public:
            DeviceMcu(const SensorIndicator&);
            virtual ~DeviceMcu();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

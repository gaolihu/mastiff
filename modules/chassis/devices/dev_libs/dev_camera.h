#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceCamera : public DeviceBaseItf {
        public:
            DeviceCamera(const SensorIndicator&);
            virtual ~DeviceCamera();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

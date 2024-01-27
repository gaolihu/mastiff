#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceLidar : public DeviceBaseItf {
        public:
            DeviceLidar(const SensorIndicator&);
            virtual ~DeviceLidar();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

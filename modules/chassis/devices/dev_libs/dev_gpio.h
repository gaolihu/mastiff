#pragma once

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceGpio : public DeviceBaseItf {
        public:
            DeviceGpio(const ChassisConfig*);
            virtual ~DeviceGpio();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

#pragma once

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceWireless : public DeviceBaseItf {
        public:
            DeviceWireless(const ChassisConfig*,
                    const SensorInfo&,
                    const SensorIndicator&);
            virtual ~DeviceWireless();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

#pragma once

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceIMU : public DeviceBaseItf {
        public:
            DeviceIMU(const ChassisConfig*,
                    const SensorInfo&,
                    const SensorIndicator&);
            virtual ~DeviceIMU();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

#pragma once

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DevServo : public DeviceBaseItf {
        public:
            DevServo(const ChassisConfig*);
            virtual ~DevServo();

            virtual int Init(void) override;
            virtual int Start(void) override;
            //Close TODO
    };

} //namespace device
} //namespace chss
} //namespace mstf

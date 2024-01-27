#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DevServo : public DeviceBaseItf {
        public:
            DevServo(const SensorIndicator&);
            virtual ~DevServo();

            virtual int Init(void) override;
            virtual int Start(void) override;
            virtual int Stop(void) override;
            virtual int Resume(void) override;
            virtual int Close(void) override;

            virtual int SetSpeed(const ChsMovementCtrl&) override;
    };

} //namespace device
} //namespace chss
} //namespace mstf

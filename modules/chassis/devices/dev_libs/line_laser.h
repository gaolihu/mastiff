#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class LineLaser : public DeviceBaseItf {
        public:
            LineLaser(const SensorIndicator&);
            virtual ~LineLaser();

            virtual int Init(void) override;
            virtual int Start(void) override;
            virtual int Stop(void) override;
    };

} //namespace device
} //namespace chss
} //namespace mstf

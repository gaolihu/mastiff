#pragma once

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceWireless : public DeviceBaseItf {
        public:
            DeviceWireless(const SensorIndicator&);
            virtual ~DeviceWireless();

            virtual int SetMisc(const ChsSocMiscCtrl&) override;

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

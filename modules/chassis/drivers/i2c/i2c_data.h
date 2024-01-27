#pragma once

#include "modules/chassis/drivers/driver_data_itf.h"

namespace mstf {
namespace chss {
namespace driver {

    class IIcData : public DriveDataItf {
        public:
            IIcData(const DriveDataMonitor&,
                    const I2cConfigs*,
                    const SensorIndicator*);
            virtual ~IIcData();

            virtual int Init(const std::string& = "",
                    const int = -1) override;

            int WritePort(const uint8_t*, const size_t) override;

            virtual int Init(const SensorIndicator*) override;
            virtual int Start(const SensorIndicator*) override;
            virtual int Stop(const SensorIndicator*) override;
            virtual int Resume(const SensorIndicator*) override;
            virtual int Close(const SensorIndicator*) override;

        private:
            virtual void PollingDriveRutine() override;

        private:
            const I2cConfigs* i2c_conf_ {};
    };

} //namespace driver
} //namespace chss
} //namespace mstf

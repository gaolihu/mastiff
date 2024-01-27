#pragma once

#include "modules/chassis/drivers/driver_data_itf.h"

namespace mstf {
namespace chss {
namespace driver {

    class CanData : public DriveDataItf {
        public:
            CanData(const DriveDataMonitor&,
                    const CanConfigs*,
                    const SensorIndicator*);
            virtual ~CanData();

            //TODO
            virtual int Init(const std::string& = "",
                    const int = 0) override;

            virtual int WritePort(const
                    std::vector<uint8_t>&) override;
            virtual int WritePort(const int,
                    const uint8_t*, const size_t) override;

            virtual int Init(const SensorIndicator*) override;
            virtual int Start(const SensorIndicator*) override;
            virtual int Stop(const SensorIndicator*) override;
            virtual int Resume(const SensorIndicator*) override;
            virtual int Close(const SensorIndicator*) override;

        private:
            virtual void PollingDriveRutine() override;

        private:
            const CanConfigs* can_conf_ {};
    };

} // namespace driver
} // namespace chss
} // namespace mstf

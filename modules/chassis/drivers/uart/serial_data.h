#pragma once

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/drivers/driver_data_itf.h"

// port serial driver
#include "modules/chassis/drivers/uart/ChannelDevice.h"
#include "modules/chassis/drivers/uart/serial.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace /*mstf::chss::*/proto;

    class SerialData : public DriveDataItf {
        public:
            SerialData(const DriveDataMonitor&,
                    const UartConfigs*,
                    const SensorIndicator*);
            ~SerialData();

            virtual int Init(const std::string& = "",
                    const int = 0) override;

            int WritePort(const uint8_t*, const size_t) override;

            virtual int Init(const SensorIndicator*) override;
            virtual int Start(const SensorIndicator*) override;
            virtual int Stop(const SensorIndicator*) override;
            virtual int Resume(const SensorIndicator*) override;
            virtual int Close(const SensorIndicator*) override;

        private:
            virtual void PollingDriveRutine() override;

        private:
            const UartConfigs* uart_conf_ {};

#ifdef SERIAL_PORTING
            ChannelDevice* comm_ = nullptr;
#endif
    };

} //namespace driver
} //namespace chss
} //namespace mstf

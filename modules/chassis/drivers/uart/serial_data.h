#pragma once

#include <functional>

#include "cyber/timer/timer.h"

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/drivers/driver_data_itf.h"

// port serial driver
#include "modules/chassis/drivers/uart/ChannelDevice.h"
#include "modules/chassis/drivers/uart/serial.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace /*mstf::chss::*/proto;

    using UartDataListener = std::function<void
        (const uint8_t*, const size_t)>;

    class SerialData : public DriveDataItf {
        public:
            SerialData(const DriveDataMonitor&,
                    const proto::UartConfig*);
            ~SerialData();

            virtual int Init(const int = 0, const
                    DriveDataPolling& = nullptr) override;
            int Push(const uint8_t*, const size_t);
            int Start();
            int Stop();
            int Close();

        private:
            int serial_fd_ = -1;
            const UartConfig* uart_conf_ = nullptr;

            void serial_poll_func();
            DriveDataMonitor serial_monitor_ = nullptr;

#ifdef SERIAL_PORTING
            ChannelDevice* comm_ = nullptr;
#endif
    };

} //namespace driver
} //namespace chss
} //namespace mstf

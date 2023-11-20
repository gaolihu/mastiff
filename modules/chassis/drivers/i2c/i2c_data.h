#pragma once

#include <functional>

#include "cyber/timer/timer.h"

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/drivers/driver_data_itf.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace /*mstf::chss::*/proto;

    using IIcDataListener = std::function<void
        (const uint8_t*, const size_t)>;

    class IIcData : public DriveDataItf {
        public:
            IIcData(const DriveDataMonitor&,
                    const I2cConfig*,
                    const SensorInfo*);
            ~IIcData();

            virtual int Init(const std::string& = "",
                    const int = 0, const
                    DriveDataPolling& = nullptr) override;
            int Push(const uint8_t*, const size_t);
            int Start();
            int Stop();
            int Resume();
            int Close();

        private:
            int i2c_fd_ = -1;
            const I2cConfig* i2c_conf_ = nullptr;
            const SensorInfo* snsr_info_ = nullptr;

            void i2c_poll_func();
            DriveDataMonitor i2c_monitor_ = nullptr;
    };

} //namespace driver
} //namespace chss
} //namespace mstf

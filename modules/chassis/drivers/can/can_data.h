#pragma once

#include "cyber/timer/timer.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/drivers/driver_data_itf.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace /*mstf::chss::*/proto;

    using CanDataListener = std::function<void
        (const uint8_t*, const size_t)>;

    class CanData : public DriveDataItf {
        public:
            CanData(const DriveDataMonitorCan&,
                    const proto::CanConfig* c);
            ~CanData();

            virtual int Init(const std::string& = "",
                    const int = 0, const
                    DriveDataPolling& = nullptr) override;
            virtual int Start() override;
            virtual int Stop() override;
            virtual int Resume() override;
            virtual int Close() override;

            virtual int WritePort(const std::vector<uint8_t>&) override;
            virtual int WritePort(const int, const uint8_t*,
                    const size_t) override;

        private:
            int can_socket_fd_ = -1;
            const CanConfig* can_conf_ = nullptr;

            void can_poll_func();
            DriveDataMonitorCan can_monitor_ = nullptr;
    };

} // namespace driver
} // namespace chss
} // namespace mstf

#pragma once

#include "cyber/timer/timer.h"

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/input_output_chs.pb.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace apollo;
    using namespace google;
    using namespace protobuf;
    using namespace /*mstf::chss::*/proto;

    using SocDataListener = std::function<void
        (const Message&)>;

    class SocData {
        public:
            SocData(const SocDataListener&);
            virtual ~SocData();

            int Init(const ChassisConfig*);
            //int Start(proto::EE_DEVICE_TYPE, const std::string = "");
            int Start();
            int Stop(proto::EE_DEVICE_TYPE, const std::string = "");
            int Close(proto::EE_DEVICE_TYPE, const std::string = "");

            int SocWrite(const Message&);

        private:
            std::string dev_;

            //SocMessageListener soc_listener_ = nullptr;
            //GpioMessageListener gpio_listener_ = nullptr;

            //std::vector<GpioHw*> gpio_hws_;
            SocDataListener soc_listner_ = nullptr;

            const ChassisConfig* chs_conf_ = nullptr;
    };

} //namespace driver
} //namespace chas
} //namespace mstf

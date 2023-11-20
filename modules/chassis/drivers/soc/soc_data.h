#pragma once

#include "cyber/timer/timer.h"

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/input_output_chs.pb.h"

#include "network/wifi_thread.h"

#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_demo.h"
#include "modules/chassis/drivers/soc/soc_defs.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace apollo;
    using namespace google;
    using namespace protobuf;
    using namespace /*mstf::chss::*/proto;

    class SocData {
        public:
            SocData(const SocDataListener&);
            virtual ~SocData();

            int Init(const SensorInfo* si = nullptr,
                    const ChassisConfig* = nullptr);
            int Start(const SensorInfo* si);
            int Stop(const SensorInfo* si);
            int Close(const SensorInfo* si);

            int SocWrite(const Message&);

        private:
            std::string dev_;

            //SocMessageListener soc_listener_ = nullptr;
            //GpioMessageListener gpio_listener_ = nullptr;

            //std::vector<GpioHw*> gpio_hws_;
            SocDataListener soc_listner_ = nullptr;

            std::shared_ptr<network::WiFiThread>
                wifi_thread_; // wifi control
            Demo camera_ctrl_; // camera control

            const ChassisConfig* chs_conf_ = nullptr;
    };

} //namespace driver
} //namespace chas
} //namespace mstf

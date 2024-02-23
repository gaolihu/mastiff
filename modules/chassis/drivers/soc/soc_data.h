#pragma once

#include "modules/chassis/proto/chss_io.pb.h"
#include "modules/chassis/drivers/driver_data_itf.h"

#include "modules/chassis/drivers/soc/js/joystick_data.h"
#include "modules/chassis/drivers/soc/audio/audio_ctrl.h"
#include "modules/chassis/drivers/soc/network/wifi_thread.h"
#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_camera_ctrl.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace apollo;
    using namespace google;
    using namespace protobuf;
    using namespace /*mstf::chss::*/proto;

    class SocData : public DriveDataItf {
        public:
            SocData(const SocDataListener&,
                    const std::shared_ptr<ChassisConfig>&);
            virtual ~SocData();

            virtual int Init(const SensorIndicator*) override;
            virtual int Start(const SensorIndicator*) override;
            virtual int Stop(const SensorIndicator*) override;
            virtual int Close(const SensorIndicator*) override;
            virtual int Resume(const SensorIndicator*) override;

            int SocWrite(const SensorIndicator*, const Message&);

        private:
            virtual void PollingDriveRutine() override;

        private:
            std::string dev_;

            SocDataListener soc_listner_ = nullptr;
            /*
               =ParseDrvLink::OnRecvSoc(
               const SensorIndicator*,
               const Message&,
               const uint8_t* = nullptr,
               const size_t = 0);
               */

            std::pair<const SensorIndicator*,
                std::shared_ptr<JoyStickData>> js_ {};
            std::pair<const SensorIndicator*,
                std::shared_ptr<AudioCtrl>> audio_ctrl_ {};
            std::pair<const SensorIndicator*,
                std::shared_ptr<WiFiThread>> wifi_ctrl_{};
            std::pair<const SensorIndicator*,
                std::shared_ptr<AsCameraCtrl>> camera_ctrl_{};

            //GpioMessageListener gpio_listener_ = nullptr;
            //std::vector<GpioHw*> gpio_hws_;

            //std::weak_ptr<ChassisConfig> chs_conf_ {};
            bool js_working_ { false };
            bool audio_working_ { false };
            bool wifi_working_ { false };
            bool camera_working_ { false };
    };

} //namespace driver
} //namespace chas
} //namespace mstf

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

            virtual int Init(const SensorIndicator* si) override;
            virtual int Start(const SensorIndicator* si) override;
            virtual int Stop(const SensorIndicator* si) override;
            virtual int Close(const SensorIndicator* si) override;
            virtual int Resume(const SensorIndicator* si) override;

            int SocWrite(const Message&);

        private:
            virtual void PollingDriveRutine() override;

        private:
            std::string dev_;

            SocDataListener soc_listner_ = nullptr;

            std::shared_ptr<JoyStickData> js_ {};
            std::shared_ptr<AudioCtrl> audio_ctrl_ {};
            std::shared_ptr<WiFiThread> wifi_ctrl_{}; // wifi control
            std::shared_ptr<AsCameraCtrl> camera_ctrl_{};

            //GpioMessageListener gpio_listener_ = nullptr;
            //std::vector<GpioHw*> gpio_hws_;


            std::weak_ptr<ChassisConfig> chs_conf_ {};
    };

} //namespace driver
} //namespace chas
} //namespace mstf

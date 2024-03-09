#pragma once

#include "cyber/common/macros.h"
#include "cyber/common/macros.h"

#include "modules/chassis/proto/chassis_config.pb.h"

namespace mstf {
namespace chss {
namespace parser {

    using namespace /*mstf::chss::*/proto;
    using namespace google::protobuf;

    class CommonItf {
        public:
            DECLARE_SINGLETON(CommonItf)
            virtual ~CommonItf();

        public:
            void SetChasisConfig(const
                    std::shared_ptr<ChassisConfig>&);
            std::shared_ptr<ChassisConfig> GetChasisConfig();

            const Message* GetDeviceConfig(const
                    SensorIndicator*) const;

            const CircularBufSetting* GetCircleBuff(const
                    SensorIndicator*) const;

            const GpioConfigs* GetDevGpioConf(const
                    SensorIndicator*) const;

            const AdcConfigs* GetDevAdcConf(const
                    SensorIndicator*) const;

            const PwmConfigs* GetDevPwmConf(const
                    SensorIndicator*) const;

            const UartConfigs* GetDevUartConf(const
                    SensorIndicator*) const;

            const I2cConfigs* GetDevI2cConf(const
                    SensorIndicator*) const;

            const CanConfigs* GetDevCanConf(const
                    SensorIndicator*) const;

            const JoyConfigs* GetDevJsConf(const
                    SensorIndicator*) const;

            const AudioConfig* GetDevAudConf(const
                    SensorIndicator*) const;

            const std::string GetCommPort(const
                    SensorIndicator*) const;

            const bool HasSocDevice() const;

            const bool IsLacateSoc(const
                    SensorIndicator*) const;

            const std::string GetTopic1(const
                    SensorIndicator*) const;

            const std::string GetTopic2(const
                    SensorIndicator*) const;

            const std::string GetTopic3(const
                    SensorIndicator*) const;

            const std::string GetTopic4(const
                    SensorIndicator*) const;

            const std::string GetTopic5(const
                    SensorIndicator*) const;

        private:
            std::shared_ptr<ChassisConfig> chss_conf_ {};
    };

} //namespace parser
} //namespace chss
} //namespace mstf

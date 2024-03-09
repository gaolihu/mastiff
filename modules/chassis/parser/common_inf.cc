#include "modules/chassis/parser/common_inf.h"

namespace mstf {
namespace chss {
namespace parser {

    CommonItf::CommonItf() {
    }

    CommonItf::~CommonItf() {
    }

    void CommonItf::SetChasisConfig(const
            std::shared_ptr<ChassisConfig>& cc) {
        chss_conf_ = cc;
    }

    std::shared_ptr<ChassisConfig>
        CommonItf::GetChasisConfig() {
        return chss_conf_;
    }

    const Message* CommonItf::GetDeviceConfig(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->gpio_dev().size(); i++)
            if (&chss_conf_->gpio_dev(i).sn_ind() == sidc)
                return &chss_conf_->gpio_dev(i);

        for (int i = 0; i < chss_conf_->adc_dev().size(); i++)
            if (&chss_conf_->adc_dev(i).sn_ind() == sidc)
                return &chss_conf_->adc_dev(i);

        for (int i = 0; i < chss_conf_->pwm_dev().size(); i++)
            if (&chss_conf_->pwm_dev(i).sn_ind() == sidc)
                return &chss_conf_->pwm_dev(i);

        for (int i = 0; i < chss_conf_->lidar_dev().size(); i++)
            if (&chss_conf_->lidar_dev(i).sn_ind() == sidc)
                return &chss_conf_->lidar_dev(i);

        for (int i = 0; i < chss_conf_->imu_dev().size(); i++)
            if (&chss_conf_->imu_dev(i).sn_ind() == sidc)
                return &chss_conf_->imu_dev(i);

        for (int i = 0; i < chss_conf_->dtof_dev().size(); i++)
            if (&chss_conf_->dtof_dev(i).sn_ind() == sidc)
                return &chss_conf_->dtof_dev(i);

        for (int i = 0; i < chss_conf_->camera_dev().size(); i++)
            if (&chss_conf_->camera_dev(i).sn_ind() == sidc)
                return &chss_conf_->camera_dev(i);

        for (int i = 0; i < chss_conf_->mcu_dev().size(); i++)
            if (&chss_conf_->mcu_dev(i).sn_ind() == sidc)
                return &chss_conf_->mcu_dev(i);

        for (int i = 0; i < chss_conf_->aud_dev().size(); i++)
            if (&chss_conf_->aud_dev(i).sn_ind() == sidc)
                return &chss_conf_->aud_dev(i);

        for (int i = 0; i < chss_conf_->servo_dev().size(); i++)
            if (&chss_conf_->servo_dev(i).sn_ind() == sidc)
                return &chss_conf_->servo_dev(i);

        for (int i = 0; i < chss_conf_->wireless_dev().size(); i++)
            if (&chss_conf_->wireless_dev(i).sn_ind() == sidc)
                return &chss_conf_->wireless_dev(i);

        for (int i = 0; i < chss_conf_->linelaser_dev().size(); i++)
            if (&chss_conf_->linelaser_dev(i).sn_ind() == sidc)
                return &chss_conf_->linelaser_dev(i);

        for (int i = 0; i < chss_conf_->joystick_dev().size(); i++)
            if (&chss_conf_->joystick_dev(i).sn_ind() == sidc)
                return &chss_conf_->joystick_dev(i);

        return nullptr;
    }

    const CircularBufSetting* CommonItf::GetCircleBuff(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->gpio_dev().size(); i++)
            if (&chss_conf_->gpio_dev(i).sn_ind() == sidc)
                return nullptr;

        for (int i = 0; i < chss_conf_->adc_dev().size(); i++)
            if (&chss_conf_->adc_dev(i).sn_ind() == sidc)
                return nullptr;

        for (int i = 0; i < chss_conf_->pwm_dev().size(); i++)
            if (&chss_conf_->pwm_dev(i).sn_ind() == sidc)
                return nullptr;

        for (int i = 0; i < chss_conf_->lidar_dev().size(); i++)
            if (&chss_conf_->lidar_dev(i).sn_ind() == sidc)
                if (chss_conf_->lidar_dev(i).has_uart_configs()) {
                    return &chss_conf_->lidar_dev(i).uart_configs().buf_setting();
                }

        for (int i = 0; i < chss_conf_->imu_dev().size(); i++)
            if (&chss_conf_->imu_dev(i).sn_ind() == sidc) {
                if (chss_conf_->imu_dev(i).has_uart_configs()) {
                    return &chss_conf_->imu_dev(i).uart_configs().buf_setting();
                } else if (chss_conf_->imu_dev(i).has_i2c_configs()) {
                    return &chss_conf_->imu_dev(i).i2c_configs().buf_setting();
                }
            }

        for (int i = 0; i < chss_conf_->dtof_dev().size(); i++)
            if (&chss_conf_->dtof_dev(i).sn_ind() == sidc)
                return nullptr;

        for (int i = 0; i < chss_conf_->camera_dev().size(); i++)
            if (&chss_conf_->camera_dev(i).sn_ind() == sidc)
                return nullptr;

        for (int i = 0; i < chss_conf_->mcu_dev().size(); i++)
            if (&chss_conf_->mcu_dev(i).sn_ind() == sidc)
                if (chss_conf_->mcu_dev(i).has_uart_configs()) {
                    return &chss_conf_->mcu_dev(i).uart_configs().buf_setting();
                }

        for (int i = 0; i < chss_conf_->aud_dev().size(); i++)
            if (&chss_conf_->aud_dev(i).sn_ind() == sidc)
                return nullptr;

        for (int i = 0; i < chss_conf_->servo_dev().size(); i++)
            if (&chss_conf_->servo_dev(i).sn_ind() == sidc)
                if (chss_conf_->servo_dev(i).has_can_configs()) {
                    return &chss_conf_->servo_dev(i).can_configs().buf_setting();
                }

        for (int i = 0; i < chss_conf_->wireless_dev().size(); i++)
            if (&chss_conf_->wireless_dev(i).sn_ind() == sidc)
                return nullptr;

        for (int i = 0; i < chss_conf_->linelaser_dev().size(); i++)
            if (&chss_conf_->linelaser_dev(i).sn_ind() == sidc)
                if (chss_conf_->linelaser_dev(i).has_uart_configs()) {
                    return &chss_conf_->linelaser_dev(i).uart_configs().buf_setting();
                }

        for (int i = 0; i < chss_conf_->joystick_dev().size(); i++)
            if (&chss_conf_->joystick_dev(i).sn_ind() == sidc)
                return nullptr;

        return nullptr;
    }

    const GpioConfigs* CommonItf::GetDevGpioConf(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->gpio_dev().size(); i++)
            if (&chss_conf_->gpio_dev(i).sn_ind() == sidc &&
                    chss_conf_->gpio_dev(i).has_gpio_configs())
                return &chss_conf_->gpio_dev(i).gpio_configs();

        return nullptr;
    }

    const AdcConfigs* CommonItf::GetDevAdcConf(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->adc_dev().size(); i++)
            if (&chss_conf_->adc_dev(i).sn_ind() == sidc &&
                    chss_conf_->adc_dev(i).has_adc_configs())
                return &chss_conf_->adc_dev(i).adc_configs();

        return nullptr;
    }

    const PwmConfigs* CommonItf::GetDevPwmConf(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->pwm_dev().size(); i++)
            if (&chss_conf_->pwm_dev(i).sn_ind() == sidc &&
                    chss_conf_->pwm_dev(i).has_pwm_configs())
                return &chss_conf_->pwm_dev(i).pwm_configs();

        return nullptr;
    }

    const UartConfigs* CommonItf::GetDevUartConf(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->lidar_dev().size(); i++)
            if (&chss_conf_->lidar_dev(i).sn_ind() == sidc &&
                    chss_conf_->lidar_dev(i).has_uart_configs())
                return &chss_conf_->lidar_dev(i).uart_configs();

        for (int i = 0; i < chss_conf_->imu_dev().size(); i++)
            if (&chss_conf_->imu_dev(i).sn_ind() == sidc &&
                    chss_conf_->imu_dev(i).has_uart_configs())
                return &chss_conf_->imu_dev(i).uart_configs();

        for (int i = 0; i < chss_conf_->mcu_dev().size(); i++)
            if (&chss_conf_->mcu_dev(i).sn_ind() == sidc &&
                    chss_conf_->mcu_dev(i).has_uart_configs())
                return &chss_conf_->mcu_dev(i).uart_configs();

        for (int i = 0; i < chss_conf_->linelaser_dev().size(); i++)
            if (&chss_conf_->linelaser_dev(i).sn_ind() == sidc &&
                    chss_conf_->linelaser_dev(i).has_uart_configs())
                return &chss_conf_->linelaser_dev(i).uart_configs();

        return nullptr;
    }

    const I2cConfigs* CommonItf::GetDevI2cConf(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->imu_dev().size(); i++)
            if (&chss_conf_->imu_dev(i).sn_ind() == sidc &&
                    chss_conf_->imu_dev(i).has_i2c_configs())
                return &chss_conf_->imu_dev(i).i2c_configs();

        return nullptr;
    }

    const CanConfigs* CommonItf::GetDevCanConf(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->servo_dev().size(); i++)
            if (&chss_conf_->servo_dev(i).sn_ind() == sidc &&
                    chss_conf_->servo_dev(i).has_can_configs())
                return &chss_conf_->servo_dev(i).can_configs();

        return nullptr;
    }

    const JoyConfigs* CommonItf::GetDevJsConf(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->joystick_dev().size(); i++)
            if (&chss_conf_->joystick_dev(i).sn_ind() == sidc &&
                    chss_conf_->joystick_dev(i).has_joy_conf())
                return &chss_conf_->joystick_dev(i).joy_conf();

        return nullptr;
    }

    const AudioConfig* CommonItf::GetDevAudConf(const
            SensorIndicator* sidc) const {
        if (!chss_conf_)
            return nullptr;

        for (int i = 0; i < chss_conf_->aud_dev().size(); i++)
            if (&chss_conf_->aud_dev(i).sn_ind() == sidc &&
                    chss_conf_->aud_dev(i).has_audio_conf())
                return &chss_conf_->aud_dev(i).audio_conf();

        return nullptr;
    }

    const std::string CommonItf::GetCommPort(const
            SensorIndicator* si) const {
        if (!chss_conf_)
            return "";

        switch (si->port()) {
            case E_COMM_PORT_SERIAL:
                return "serial";
            case E_COMM_PORT_485:
                return "485";
            case E_COMM_PORT_232:
                return "232";
            case E_COMM_PORT_CAN:
                return "can";
            case E_COMM_PORT_I2C:
                return "i2c";
            case E_COMM_PORT_SPI:
                return "spi";
            case E_COMM_PORT_USB:
                return "usb";
            case E_COMM_PORT_MIPI:
                return "mipi";
            case E_COMM_PORT_GPIO:
                return "gpio";
            case E_COMM_PORT_I2S:
                return "i2s";
            case E_COMM_PORT_SDIO:
                return "sdio";
            case E_COMM_PORT_24G:
                return "2.4G";
            default:
            //case E_COMM_PORT_SENTINEL:
                return "ERROR";
        }
    }

    const bool CommonItf::HasSocDevice() const {
        if (!chss_conf_)
            return false;

        for (int i = 0; i < chss_conf_->gpio_dev().size(); i++)
            if (chss_conf_->gpio_dev(i).sn_ind().dev_main() ==
                    EE_DEV_MAIN_SOC && chss_conf_->gpio_dev(i).used())
                return true;

        for (int i = 0; i < chss_conf_->camera_dev().size(); i++)
            if (chss_conf_->camera_dev(i).sn_ind().dev_main() ==
                    EE_DEV_MAIN_SOC && chss_conf_->camera_dev(i).used())
                return true;

        for (int i = 0; i < chss_conf_->aud_dev().size(); i++)
            if (chss_conf_->aud_dev(i).sn_ind().dev_main() ==
                    EE_DEV_MAIN_SOC && chss_conf_->aud_dev(i).used())
                return true;

        for (int i = 0; i < chss_conf_->wireless_dev().size(); i++)
            if (chss_conf_->wireless_dev(i).sn_ind().dev_main() ==
                    EE_DEV_MAIN_SOC && chss_conf_->wireless_dev(i).used())
                return true;

        for (int i = 0; i < chss_conf_->joystick_dev().size(); i++)
            if (chss_conf_->joystick_dev(i).sn_ind().dev_main() ==
                    EE_DEV_MAIN_SOC && chss_conf_->joystick_dev(i).used())
                return true;

        return false;
    }

    const bool CommonItf::IsLacateSoc(const
            SensorIndicator* sidc) const {
        return (sidc->dev_main() == EE_DEV_MAIN_SOC);
    }

    const std::string CommonItf::GetTopic1(const
            SensorIndicator* sidc) const {
        return (sidc->ihi().topic1());
    }

    const std::string CommonItf::GetTopic2(const
            SensorIndicator* sidc) const {
        return (sidc->ihi().topic2());
    }

    const std::string CommonItf::GetTopic3(const
            SensorIndicator* sidc) const {
        return (sidc->ihi().topic3());
    }

    const std::string CommonItf::GetTopic4(const
            SensorIndicator* sidc) const {
        return (sidc->ihi().topic4());
    }

    const std::string CommonItf::GetTopic5(const
            SensorIndicator* sidc) const {
        return (sidc->ihi().topic5());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

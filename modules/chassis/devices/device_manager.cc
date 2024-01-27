#include "cyber/common/log.h"

#include "modules/chassis/devices/transactor.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/device_manager.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceManager::DeviceManager(const
            std::shared_ptr<ChassisConfig>& cc) {
        ACHECK(cc != nullptr) << "ChassisConfigPtr NULL!";

#ifdef CHSS_PKG_DBG
        AINFO << "~~~~~ DeviceManager construct start ~~~~~";
        auto start_time = std::chrono::steady_clock::now();
#endif
        chs_conf_ = cc;
        AINFO << "start instantiate chs peripherals";

        Transactor::Instance()->Init(chs_conf_.lock());

        //1, servo motor
        for (int i = 0; i < chs_conf_.lock()->servo_dev().size(); i++) {
            if (chs_conf_.lock()->servo_dev(i).used()) {
                servo_.emplace_back(std::make_unique<DevServo>(
                            chs_conf_.lock()->servo_dev(i).sn_ind()));
            }
        }

        //2, lidar device
        for (int i = 0; i < chs_conf_.lock()->lidar_dev().size(); i++) {
            if (chs_conf_.lock()->lidar_dev(i).used()) {
                dev_lidar_.emplace_back(std::make_unique<DeviceLidar>(
                            chs_conf_.lock()->lidar_dev(i).sn_ind()));
            }
        }

        //3, linelaser device
        for (int i = 0; i < chs_conf_.lock()->linelaser_dev().size(); i++) {
            if (chs_conf_.lock()->linelaser_dev(i).used()) {
                line_laser_.emplace_back(std::make_unique<LineLaser>(
                            chs_conf_.lock()->linelaser_dev(i).sn_ind()));
            }
        }

        //4, IMU device
        for (int i = 0; i < chs_conf_.lock()->imu_dev().size(); i++) {
            if (chs_conf_.lock()->imu_dev(i).used()) {
                dev_imu_.emplace_back(std::make_unique<DeviceIMU>(
                            chs_conf_.lock()->imu_dev(i).sn_ind()));
            }
        }

        //5, Joystick
        for (int i = 0; i < chs_conf_.lock()->joystick_dev().size(); i++) {
            if (chs_conf_.lock()->joystick_dev(i).used()) {
                dev_joy_.emplace_back(std::make_unique<DeviceJoyStick>(
                            chs_conf_.lock()->joystick_dev(i).sn_ind()));
            }
        }

        //6, audio device
        for (int i = 0; i < chs_conf_.lock()->aud_dev().size(); i++) {
            if (chs_conf_.lock()->aud_dev(i).used()) {
                audio_paly_.emplace_back(std::make_unique<AudioPlay>(
                            chs_conf_.lock()->aud_dev(i).sn_ind()));

            }
        }

        //7, wireless device
        for (int i = 0; i < chs_conf_.lock()->wireless_dev().size(); i++) {
            if (chs_conf_.lock()->wireless_dev(i).used()) {
                dev_wireless_.emplace_back(std::make_unique<DeviceWireless>(
                            chs_conf_.lock()->wireless_dev(i).sn_ind()));
            }
        }

        //8, camera device
        for (int i = 0; i < chs_conf_.lock()->camera_dev().size(); i++) {
            if (chs_conf_.lock()->camera_dev(i).used()) {
                dev_camera_.emplace_back(std::make_unique<DeviceCamera>(
                            chs_conf_.lock()->camera_dev(i).sn_ind()));
            }
        }

#if 0
        //5, mcu device
        for (int i = 0; i < chs_conf_.lock()->mcu_dev().size(); i++) {
            if (chs_conf_.lock()->mcu_dev(i).used()) {
                dev_mcu_.emplace_back(std::make_unique<DeviceMcu>(cc.get(),
                            chs_conf_.lock()->mcu_dev(i).si(),
                            chs_conf_.lock()->mcu_dev(i).sn_ind()));
            }
        }

        //7, gpio device
        for (int i = 0; i < chs_conf_.lock()->gpio_dev().size(); i++) {
            if (chs_conf_.lock()->gpio_dev(i).used()) {
                dev_gpio_.emplace_back(std::make_unique<DeviceGpio>(cc.get(),
                            chs_conf_.lock()->gpio_dev(i).si(),
                            chs_conf_.lock()->gpio_dev(i).sn_ind()));
            }
        }

#endif
#ifdef CHSS_PKG_DBG
        auto end_time = std::chrono::steady_clock::now();
        AINFO << "~~~~~ DeviceManager construct take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms ~~~~~";
#endif
    }

    DeviceManager::~DeviceManager() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceManager de-construct start, chs_conf_ p: " <<
            chs_conf_.lock();
#endif
        Transactor::Instance()->Finish();
        DeviceClose();
        AINFO << "DeviceManager de-construct finished, chs_conf_ p: " <<
            chs_conf_.lock();
    }

#if 0
    void DeviceManager::ManageDevices(const DevicesManager& ma) {
        if (ma.has_gpio_m() && ma.gpio_m().index() < dev_gpio_.size()) {
            switch (ma.gpio_m().val()) {
            case E_DEVICES_MANAGE_START:
                //dev_gpio_->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                //dev_gpio_->StopSensor();
                break;
            case E_DEVICES_MANAGE_RESUME:
                //dev_gpio_->ResumeSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                //dev_gpio_->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage gpio device!";
            }
        }

        if (ma.has_mcu_m() && ma.mcu_m().index() < dev_mcu_.size()) {
            switch (ma.mcu_m().val()) {
            case E_DEVICES_MANAGE_START:
                dev_mcu_[ma.mcu_m().index()]->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                dev_mcu_[ma.mcu_m().index()]->StopSensor();
                break;
            case E_DEVICES_MANAGE_RESUME:
                dev_mcu_[ma.mcu_m().index()]->ResumeSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                dev_mcu_[ma.mcu_m().index()]->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage mcu device!";
            }
        }

        if (ma.has_servo_m() && ma.servo_m().index() < servo_.size()) {
            switch (ma.servo_m().val()) {
            case E_DEVICES_MANAGE_START:
                servo_[ma.servo_m().index()]->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                servo_[ma.servo_m().index()]->StopSensor();
                break;
            case E_DEVICES_MANAGE_RESUME:
                servo_[ma.servo_m().index()]->ResumeSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                servo_[ma.servo_m().index()]->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage servo motor!";
            }
        }

        if (ma.has_lidar_m() && ma.lidar_m().index() < dev_lidar_.size()) {
            switch (ma.lidar_m().val()) {
            case E_DEVICES_MANAGE_START:
                dev_lidar_[ma.lidar_m().index()]->StartSensor();
                //dev_lidar_[ma.lidar_m().index()]->SpeedRegulate(40, 50);
                break;
            case E_DEVICES_MANAGE_STOP:
                dev_lidar_[ma.lidar_m().index()]->StopSensor();
                //dev_lidar_[ma.lidar_m().index()]->SpeedRegulate(0, 0);
                break;
            case E_DEVICES_MANAGE_RESUME:
                dev_lidar_[ma.lidar_m().index()]->ResumeSensor();
                //dev_lidar_[ma.lidar_m().index()]->SpeedRegulate(0, 0);
                break;
            case E_DEVICES_MANAGE_CLOSE:
                dev_lidar_[ma.lidar_m().index()]->CloseSensor();
                //dev_lidar_[ma.lidar_m().index()]->SpeedRegulate(0, 0);
                break;
            default:
                AWARN << "unknown command for manage lidar device!";
            }
        }

        if (ma.has_camera_m() && ma.camera_m().index() < dev_camera_.size()) {
            switch (ma.camera_m().val()) {
            case E_DEVICES_MANAGE_START:
                dev_camera_[ma.camera_m().index()]->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                dev_camera_[ma.camera_m().index()]->StopSensor();
                break;
            case E_DEVICES_MANAGE_RESUME:
                dev_camera_[ma.camera_m().index()]->ResumeSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                dev_camera_[ma.camera_m().index()]->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage camera device!";
            }
        }

        if (ma.has_audio_m() && ma.audio_m().index() < audio_paly_.size()) {
            switch (ma.audio_m().val()) {
            case E_DEVICES_MANAGE_START:
                audio_paly_[ma.audio_m().index()]->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                audio_paly_[ma.audio_m().index()]->StopSensor();
                break;
            case E_DEVICES_MANAGE_RESUME:
                audio_paly_[ma.audio_m().index()]->ResumeSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                audio_paly_[ma.audio_m().index()]->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage audio device!";
            }
        }

        if (ma.has_linelaser_m() && ma.linelaser_m().index() < line_laser_.size()) {
            switch (ma.linelaser_m().val()) {
            case E_DEVICES_MANAGE_START:
                line_laser_[ma.linelaser_m().index()]->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                line_laser_[ma.linelaser_m().index()]->StopSensor();
                break;
            case E_DEVICES_MANAGE_RESUME:
                line_laser_[ma.linelaser_m().index()]->ResumeSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                line_laser_[ma.linelaser_m().index()]->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage linelaser device!";
            }
        }

        if (ma.has_imu_m() && ma.imu_m().index() < dev_imu_.size()) {
            switch (ma.imu_m().val()) {
            case E_DEVICES_MANAGE_START:
                dev_imu_[ma.imu_m().index()]->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                dev_imu_[ma.imu_m().index()]->StopSensor();
                break;
            case E_DEVICES_MANAGE_RESUME:
                dev_imu_[ma.imu_m().index()]->ResumeSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                dev_imu_[ma.imu_m().index()]->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage audio device!";
            }
        }

        if (ma.has_joy_m() && ma.joy_m().index() < dev_joy_.size()) {
            switch (ma.joy_m().val()) {
                case E_DEVICES_MANAGE_START:
                    dev_joy_[ma.joy_m().index()]->StartSensor();
                break;
                case E_DEVICES_MANAGE_STOP:
                    dev_joy_[ma.joy_m().index()]->StopSensor();
                break;
                case E_DEVICES_MANAGE_RESUME:
                    dev_joy_[ma.joy_m().index()]->ResumeSensor();
                break;
                case E_DEVICES_MANAGE_CLOSE:
                    dev_joy_[ma.joy_m().index()]->CloseSensor();
                break;
                default:
                AWARN << "unknown command for manage Joy Stick device!";
                break;
            }
        }
    }
#endif

    //operate all device
    int DeviceManager::DeviceInit() {
        return DevParseLink::Instance()->DeviceInit();
    }

    int DeviceManager::DeviceStart() {
        return DevParseLink::Instance()->DeviceStart();
    }

    int DeviceManager::DeviceStop() {
        return DevParseLink::Instance()->DeviceStop();
    }

    int DeviceManager::DeviceResume() {
        return DevParseLink::Instance()->DeviceResume();
    }

    int DeviceManager::DeviceClose() {
        return DevParseLink::Instance()->DeviceClose();
    }

} //namespace device
} //namespace chss
} //namespace mstf

#include "cyber/common/log.h"

#include "modules/chassis/devices/device_manager.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceManager::DeviceManager(const
            std::shared_ptr<ChassisConfig>& cc) {
        ACHECK(cc != nullptr) << "ChassisConfigPtr NULL!";

#ifdef CHSS_PKG_DBG
        AINFO << "DeviceManager construct";
#endif

        chs_conf_ = const_cast<ChassisConfig*>(cc.get());

        //1, servo motor
        if (chs_conf_->has_servo_dev() &&
                chs_conf_->servo_dev().used()) {
            servo_ = std::make_unique<DevServo>(cc.get());

            sensor_ind_infos_pair_[const_cast<SensorIndicator*>
                (&chs_conf_->servo_dev().sn_ind())] =
                dynamic_cast<DeviceBaseItf*>(servo_.get());
        }

        //2, audio device
        if (chs_conf_->has_aud_dev() &&
                chs_conf_->mutable_aud_dev()->used()) {
            audio_paly_ = std::make_unique<AudioPlay>(cc.get());

            //create SensorIndicator to bind with device
            sensor_ind_infos_pair_[chs_conf_->mutable_aud_dev()->
                mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(audio_paly_.get());
        }

        //3, wireless device
        if (chs_conf_->has_wireless_dev() &&
                chs_conf_->mutable_wireless_dev()->used()) {
            dev_wireless_ = std::make_unique<DeviceWireless>(cc.get());

            //create SensorIndicator to bind with device
            sensor_ind_infos_pair_[chs_conf_->mutable_wireless_dev()->
                mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(dev_wireless_.get());
        }

        //4, lidar device
        if (chs_conf_->has_lidar_dev() &&
                chs_conf_->mutable_lidar_dev()->used()) {
            dev_lidar_ = std::make_unique<DeviceLidar>(cc.get());

            //create SensorIndicator to bind with device
            sensor_ind_infos_pair_[chs_conf_->mutable_lidar_dev()->
                mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(dev_lidar_.get());
        }

        //5, mcu device
        if (chs_conf_->has_mcu_dev() &&
                chs_conf_->mutable_mcu_dev()->used()) {
            dev_mcu_ = std::make_unique<DeviceMcu>(cc.get());

            //create SensorIndicator to bind with device
            sensor_ind_infos_pair_[chs_conf_->mutable_mcu_dev()->
                mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(dev_mcu_.get());
        }

        //6, camera device
        if (chs_conf_->has_camera_dev() &&
                chs_conf_->mutable_camera_dev()->used()) {
            dev_camera_ = std::make_unique<DeviceCamera>(cc.get());

            //create SensorIndicator to bind with device
            sensor_ind_infos_pair_[chs_conf_->mutable_camera_dev()->
                mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(dev_camera_.get());
        }

        //7, gpio device
        if (chs_conf_->has_gpio_dev() &&
                chs_conf_->mutable_gpio_dev()->used()) {
            dev_gpio_ = std::make_unique<DeviceGpio>(cc.get());

            //create SensorIndicator to bind with device
            sensor_ind_infos_pair_[chs_conf_->mutable_gpio_dev()->
                mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(dev_gpio_.get());
        }

        //8, linelaser device
        if (chs_conf_->has_linelaser_dev() &&
                chs_conf_->mutable_linelaser_dev()->used()) {
            line_laser_ = std::make_unique<LineLaser>(cc.get());

            //create SensorIndicator to bind with device
            sensor_ind_infos_pair_[chs_conf_->mutable_linelaser_dev()->
                mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(line_laser_.get());
        }

#if 0
        auto start_time = std::chrono::steady_clock::now();
        //1, gpio
        if (chs_conf_->has_gpio_dev() &&
                chs_conf_->gpio_dev().used()) {
            gpio_ = std::make_unique<GpioDevs>(
                    chs_conf_->gpio_dev());

            sensor_ind_infos_pair_[chs_conf_->
                mutable_gpio_dev()->mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(gpio_.get());
        }

        if (chs_conf_->has_adc_dev() &&
                chs_conf_->adc_dev().used()) {
            //TODO
            AINFO << "Instantiate ADC on soc, TODO";
        }

        if (chs_conf_->has_pwm_dev() &&
                chs_conf_->mutable_pwm_dev()->used()) {
            pwm_ = std::make_unique<PwmDev>(*(chs_conf_->
                        mutable_pwm_dev()));

            sensor_ind_infos_pair_[chs_conf_->
                mutable_pwm_dev()->mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(pwm_.get());
        }

        if (chs_conf_->has_lidar_dev() &&
                chs_conf_->mutable_lidar_dev()->used()) {
            lidar_ = std::make_unique<LidarDev>(*(chs_conf_->
                        mutable_lidar_dev()));

            sensor_ind_infos_pair_[chs_conf_->
                mutable_lidar_dev()->mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(lidar_.get());
            //register switch for lidar
            if (chs_conf_->mutable_lidar_dev()->has_lidar_switch()) {
                auto ctrl = [this] (const bool sw)->int {
                    if (gpio_->Start(chs_conf_->mutable_lidar_dev()->
                                lidar_switch().gpio_pin()) == 0) {
                        return gpio_->CtrlGpio(chs_conf_->
                                mutable_lidar_dev()->
                                lidar_switch().gpio_pin(), sw);
                    }
                    return -1;
                };
                lidar_->RegisterGpioSwitch(ctrl);
            }
            //register pwm adjust for lidar
            if (chs_conf_->mutable_lidar_dev()->has_pwm_unique_id()) {
                auto ctrl = [this] (const int cycle, const int duty)->int {
#ifdef CHSS_PKG_DBG
                    AINFO << "regulate pwm, cycle: " << cycle <<
                        ", duty: " << duty;
#endif
                    return pwm_->RegulatePwm(chs_conf_->
                            mutable_lidar_dev()->
                            pwm_unique_id().value(),
                            cycle, duty);
                };
                lidar_->RegisterPwmRegulate(ctrl);
            }
        }

        if (chs_conf_->has_imu_dev() &&
                chs_conf_->mutable_imu_dev()->used()) {
            //TODO
            AINFO << "Instantiate IMU on soc, TODO";
        }

        if (chs_conf_->has_dtof_dev() &&
                chs_conf_->mutable_dtof_dev()->used()) {
            //TODO
            AINFO << "Instantiate Dtof on soc, TODO";
        }

        if (chs_conf_->has_camera_dev() &&
                chs_conf_->mutable_camera_dev()->used()) {
            camera_ = std::make_unique<CameraDev>(chs_conf_->
                        camera_dev());

            sensor_ind_infos_pair_[chs_conf_->
                mutable_camera_dev()->mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(camera_.get());
        }

        if (chs_conf_->has_mcu_dev() &&
                chs_conf_->mutable_mcu_dev()->used()) {
            mcu_ = std::make_unique<McuSlave>(chs_conf_->
                    mcu_dev());

            //create SensorIndicator to bind with device
            sensor_ind_infos_pair_[chs_conf_->mutable_mcu_dev()->
                mutable_sn_ind()] =
                dynamic_cast<DeviceBaseItf*>(mcu_.get());
        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "DeviceManager construction OK, managed dev: "
            << sensor_ind_infos_pair_.size() <<
            ", take time: " << std::chrono::duration<double,
            std::milli>(end_time - start_time).count() <<
                " ms";
#endif
    }

    DeviceManager::~DeviceManager() {
        AINFO << "DeviceManager de-construct, chs_conf_ p: " <<
            chs_conf_;

        DeviceClose();

#if 0
        mcu_.reset();

        /*
        AINFO << "DeviceManager de-construct, chs_conf_ mcu_dev: " <<
            chs_conf_->mcu_dev().DebugString();
            */

        lidar_.reset();
        camera_.reset();
        gpio_.reset();
        pwm_.reset();
        audio_.reset();
#endif

        AINFO << "DeviceManager de-construct finished, chs_conf_ p: " <<
            chs_conf_;
    }

    void DeviceManager::ManageDevices(const DevicesManager& ma) {
#if 0
        if (ma.has_gpio_m() && gpio_.get() != nullptr) {
            switch (ma.gpio_m().val()) {
            case E_DEVICES_MANAGE_START:
                //gpio_->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                //gpio_->StopSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                //gpio_->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage gpio device!";
            }
        }

        if (ma.has_mcu_m() && mcu_.get() != nullptr) {
            switch (ma.mcu_m().val()) {
            case E_DEVICES_MANAGE_START:
                //mcu_->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                //mcu_->StopSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                //mcu_->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage mcu device!";
            }
        }

        if (ma.has_lidar_m() && lidar_.get() != nullptr) {
            switch (ma.lidar_m().val()) {
            case E_DEVICES_MANAGE_START:
                //lidar_->StartSensor();
                //lidar_->SpeedRegulate(40, 50);
                break;
            case E_DEVICES_MANAGE_STOP:
                //lidar_->StopSensor();
                //lidar_->SpeedRegulate(0, 0);
                break;
            case E_DEVICES_MANAGE_CLOSE:
                //lidar_->CloseSensor();
                //lidar_->SpeedRegulate(0, 0);
                break;
            default:
                AWARN << "unknown command for manage lidar device!";
            }
        }

        if (ma.has_camera_m() && camera_.get() != nullptr) {
            switch (ma.camera_m().val()) {
            case E_DEVICES_MANAGE_START:
                //camera_->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                //camera_->StopSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                //camera_->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage camera device!";
            }
        }

        if (ma.has_audio_m() && audio_.get() != nullptr) {
            switch (ma.audio_m().val()) {
            case E_DEVICES_MANAGE_START:
                //audio_->StartSensor();
                break;
            case E_DEVICES_MANAGE_STOP:
                //audio_->StopSensor();
                break;
            case E_DEVICES_MANAGE_CLOSE:
                //audio_->CloseSensor();
                break;
            default:
                AWARN << "unknown command for manage audio device!";
            }
        }
#endif
    }

    //operate all device
    int DeviceManager::DeviceInit() {
        int i = 1, ret = 0;
        auto start_time = std::chrono::steady_clock::now();
        AINFO << "===== init all of the " <<
            sensor_ind_infos_pair_.size() <<
            " managed devices =====";

        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++, ++i) {
            DeviceBaseItf* dev_itf = it->second;
            if ((ret = dev_itf->InitSensor(it->first)) != 0) {
                AWARN << "init device: " <<
                    dev_itf->GetSensorName() << " failed!";
            }
#ifdef CHSS_PKG_DBG
            AINFO << "===init device idx: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
                (ret == 0 ? ", OK!" : ", NG!") <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "===== DeviceManager init all " <<
            sensor_ind_infos_pair_.size() << " devices, take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms =====";

        return ret;
    }

    int DeviceManager::DeviceStart() {
        int i = 1, ret = 0;
        auto start_time = std::chrono::steady_clock::now();
        AINFO << "##### start all of the " <<
            sensor_ind_infos_pair_.size() <<
            " managed devices #####";
        /*
        //make sure GPIO start first of all
        gpio_->Start();
        */

        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++, ++i) {
            DeviceBaseItf* dev_itf = it->second;
            if ((ret = dev_itf->StartSensor()) != 0) {
                AWARN << "start device: " <<
                    dev_itf->GetSensorName() << " failed!";
                ret = -1;
            }
#ifdef CHSS_PKG_DBG
            AINFO << "###start device idx: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
                (ret == 0 ? ", OK!" : ", NG!") <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "##### DeviceManager start all " <<
            sensor_ind_infos_pair_.size() << " devices, take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms #####";

        return ret;
    }

    int DeviceManager::DeviceStop() {
        int i = 1, ret = 0;
        auto start_time = std::chrono::steady_clock::now();
        AINFO << "@@@@@ stop all of the " <<
            sensor_ind_infos_pair_.size() <<
            " managed devices @@@@@";
        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++, ++i) {
            DeviceBaseItf* dev_itf = it->second;
            if ((ret = dev_itf->StopSensor()) != 0) {
                AWARN << "stop device: " <<
                    dev_itf->GetSensorName() << " failed!";
            }
#ifdef CHSS_PKG_DBG
            AINFO << "@@@stop device idx: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
                (ret == 0 ? ", OK!" : ", NG!") <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "@@@@@ DeviceManager stop all " <<
            sensor_ind_infos_pair_.size() << " devices, take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms @@@@@";

        return ret;
    }

    void DeviceManager::DeviceClose() {
        int i = 1;
        auto start_time = std::chrono::steady_clock::now();
        AINFO << "----- close all of the " <<
            sensor_ind_infos_pair_.size() <<
            " managed devices -----";
        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++, ++i) {
            auto dev_itf = it->second;
            dev_itf->CloseSensor();
#ifdef CHSS_PKG_DBG
            AINFO << "---close device idx: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "----- DeviceManager close all " <<
            sensor_ind_infos_pair_.size() << " devices, take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms -----";
    }

    //operate conclusive device
    int DeviceManager::DeviceInit(SensorIndicator* snr_ind) {
        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++) {
            auto snr_i = it->first;
            if (snr_i == snr_ind) {
                auto dev_itf = it->second;
                return dev_itf->InitSensor(snr_i);
            }
        }

        return -1;
    }

    int DeviceManager::DeviceStart(SensorIndicator* snr_ind) {
        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++) {
            auto snr_i = it->first;
            if (snr_i == snr_ind) {
                auto dev_itf = it->second;
                return dev_itf->StartSensor();
            }
        }

        return -1;
    }

    int DeviceManager::DeviceStop(SensorIndicator* snr_ind) {
        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++) {
            auto snr_i = it->first;
            if (snr_i == snr_ind) {
                auto dev_itf = it->second;
                return dev_itf->StopSensor();
            }
        }

        return -1;
    }

    void DeviceManager::DeviceClose(SensorIndicator* snr_ind) {
        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++) {
            auto snr_i = it->first;
            if (snr_i == snr_ind) {
                auto dev_itf = it->second;
                dev_itf->CloseSensor();
                return;
            }
        }
    }

    SensorIndicator* DeviceManager::GetGpioSwitchByPin(std::string& pin) {
        //gpio_
        return nullptr;
    }

} //namespace device
} //namespace chss
} //namespace mstf

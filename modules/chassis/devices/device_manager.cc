#include "cyber/common/log.h"

#include "modules/chassis/devices/device_manager.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceManager::DeviceManager(const
            std::shared_ptr<ChassisConfig>& cc) {
        ACHECK(cc != nullptr) << "ChassisConfigPtr NULL!";

#ifdef CHSS_PKG_DBG
        AINFO << "DeviceManager construct start";
#endif

        auto start_time = std::chrono::steady_clock::now();

        chs_conf_ = const_cast<ChassisConfig*>(cc.get());
        AINFO << "start to instantiate peripheral devices";

        //1, servo motor
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if (chs_conf_->servo_dev(i).used()) {
                servo_.emplace_back(std::make_unique<DevServo>(cc.get(),
                            chs_conf_->servo_dev(i).si(),
                            chs_conf_->servo_dev(i).sn_ind()));

                //create SensorIndicator to bind with device
                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->servo_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(servo_.back().get());
            }
        }

        //2, audio device
        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if (chs_conf_->aud_dev(i).used()) {
                audio_paly_.emplace_back(std::make_unique<AudioPlay>(cc.get(),
                            chs_conf_->aud_dev(i).si(),
                            chs_conf_->aud_dev(i).sn_ind()));

                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->aud_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(audio_paly_.back().get());
            }
        }

        //3, wireless device
        for (int i = 0; i < chs_conf_->wireless_dev().size(); i++) {
            if (chs_conf_->wireless_dev(i).used()) {
                dev_wireless_.emplace_back(std::make_unique<DeviceWireless>(cc.get(),
                            chs_conf_->wireless_dev(i).si(),
                            chs_conf_->wireless_dev(i).sn_ind()));

                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->wireless_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(dev_wireless_.back().get());
            }
        }

        //4, lidar device
        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if (chs_conf_->lidar_dev(i).used()) {
                dev_lidar_.emplace_back(std::make_unique<DeviceLidar>(cc.get(),
                            chs_conf_->lidar_dev(i).si(), chs_conf_->lidar_dev(i).sn_ind()));

                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->lidar_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(dev_lidar_.back().get());
            }
        }

        //5, mcu device
        for (int i = 0; i < chs_conf_->mcu_dev().size(); i++) {
            if (chs_conf_->mcu_dev(i).used()) {
                dev_mcu_.emplace_back(std::make_unique<DeviceMcu>(cc.get(),
                            chs_conf_->mcu_dev(i).si(),
                            chs_conf_->mcu_dev(i).sn_ind()));

                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->mcu_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(dev_mcu_.back().get());
            }
        }

        //6, camera device
        for (int i = 0; i < chs_conf_->camera_dev().size(); i++) {
            if (chs_conf_->camera_dev(i).used()) {
                dev_camera_.emplace_back(std::make_unique<DeviceCamera>(cc.get(),
                            chs_conf_->camera_dev(i).si(),
                            chs_conf_->camera_dev(i).sn_ind()));

                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->camera_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(dev_camera_.back().get());
            }
        }

        //7, gpio device
        for (int i = 0; i < chs_conf_->gpio_dev().size(); i++) {
            if (chs_conf_->gpio_dev(i).used()) {
                dev_gpio_.emplace_back(std::make_unique<DeviceGpio>(cc.get(),
                            chs_conf_->gpio_dev(i).si(),
                            chs_conf_->gpio_dev(i).sn_ind()));

                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->gpio_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(dev_gpio_.back().get());
            }
        }

        //8, linelaser device
        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if (chs_conf_->linelaser_dev(i).used()) {
                line_laser_.emplace_back(std::make_unique<LineLaser>(cc.get(),
                            chs_conf_->linelaser_dev(i).si(),
                            chs_conf_->linelaser_dev(i).sn_ind()));

                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->linelaser_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(dev_gpio_.back().get());
            }
        }

        //9, IMU device
        for (int i = 0; i < chs_conf_->imu_dev().size(); i++) {
            if (chs_conf_->imu_dev(i).used()) {
                dev_imu_.emplace_back(std::make_unique<DeviceIMU>(cc.get(),
                            chs_conf_->imu_dev(i).si(),
                            chs_conf_->imu_dev(i).sn_ind()));

                sensor_ind_infos_pair_[const_cast
                    <SensorIndicator*>(&chs_conf_->imu_dev(i).sn_ind())] =
                    dynamic_cast<DeviceBaseItf*>(dev_imu_.back().get());
            }
        }

        /*
        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++) {
            AINFO << "si: \n" << it->first->DebugString();
        }
        */
        auto end_time = std::chrono::steady_clock::now();
#ifdef CHSS_PKG_DBG
        AINFO << "construction OK, managed: "
            << sensor_ind_infos_pair_.size() <<
            ", take : " << std::chrono::duration<double,
            std::milli>(end_time - start_time).count() <<
                " ms";
#endif
    }

    DeviceManager::~DeviceManager() {
        AINFO << "DeviceManager de-construct start, chs_conf_ p: " <<
            chs_conf_;

        DeviceClose();
#if 0
        dev_mcu_.reset();

        /*
        AINFO << "DeviceManager de-construct, chs_conf_ mcu_dev: " <<
            chs_conf_->mcu_dev().DebugString();
            */

        dev_lidar_.reset();
        dev_camera0_.reset();
        dev_gpio_.reset();
        pwm_.reset();
        audio_paly_.reset();
        dev_imu_.reset();
#endif
        AINFO << "DeviceManager de-construct finished, chs_conf_ p: " <<
            chs_conf_;
    }

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
            AINFO << "===init device finished index: " << i <<
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
        dev_gpio_->Start();
        */

        for (auto it = sensor_ind_infos_pair_.begin();
                it != sensor_ind_infos_pair_.end(); it++, ++i) {
            DeviceBaseItf* dev_itf = it->second;
            if ((ret = dev_itf->StartSensor()) != 0) {
                AERROR << "start device: " <<
                    dev_itf->GetSensorName() << " failed!";
                ret = -1;
            }
#ifdef CHSS_PKG_DBG
            AINFO << "###start device finished index: " << i <<
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
            AINFO << "@@@stop device finished index: " << i <<
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
            AINFO << "---close device finished index: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
                std::this_thread::sleep_for(std::chrono::
                        milliseconds(100));
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
        //dev_gpio_
        return nullptr;
    }

} //namespace device
} //namespace chss
} //namespace mstf

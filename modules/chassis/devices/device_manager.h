#pragma once

#include <unordered_map>

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/input_output_chs.pb.h"

#include "modules/chassis/devices/dev_libs/dev_servo.h"
#include "modules/chassis/devices/dev_libs/audio_play.h"
#include "modules/chassis/devices/dev_libs/dev_wireless.h"
#include "modules/chassis/devices/dev_libs/dev_lidar.h"
#include "modules/chassis/devices/dev_libs/dev_mcu.h"
#include "modules/chassis/devices/dev_libs/dev_camera.h"
#include "modules/chassis/devices/dev_libs/dev_gpio.h"
#include "modules/chassis/devices/dev_libs/line_laser.h"
#include "modules/chassis/devices/dev_libs/dev_imu.h"

namespace mstf {
namespace chss {
namespace device {

    using namespace /*mstf::chss::*/proto;

    class DeviceManager {
        public:
            DeviceManager(const
                    std::shared_ptr<ChassisConfig>&);
            virtual ~DeviceManager();

            void ManageDevices(const DevicesManager&);

            int DeviceInit();
            int DeviceStart();
            int DeviceStop();
            void DeviceClose();

            SensorIndicator* GetGpioSwitchByPin(std::string&);

        private:
            int DeviceInit(SensorIndicator*);
            int DeviceStart(SensorIndicator*);
            int DeviceStop(SensorIndicator*);
            void DeviceClose(SensorIndicator*);

        private:
            //std::shared_ptr<ChassisConfig*> chs_conf_ = nullptr;
            ChassisConfig* chs_conf_ = nullptr;

            std::vector<std::unique_ptr<DevServo>> servo_ = {};
            std::vector<std::unique_ptr<AudioPlay>> audio_paly_ = {};
            std::vector<std::unique_ptr<DeviceWireless>> dev_wireless_  = {};
            std::vector<std::unique_ptr<DeviceLidar>> dev_lidar_ = {};
            std::vector<std::unique_ptr<DeviceMcu>> dev_mcu_ = {};
            std::vector<std::unique_ptr<DeviceCamera>> dev_camera_ = {};
            std::vector<std::unique_ptr<DeviceGpio>> dev_gpio_ = {};
            std::vector<std::unique_ptr<LineLaser>> line_laser_ = {};
            std::vector<std::unique_ptr<DeviceIMU>> dev_imu_ = {};

            //devices & device indicator map
            std::unordered_map<SensorIndicator*, DeviceBaseItf*>
                sensor_ind_infos_pair_;
    };

} //namespace device
} //namespace chss
} //namespace mstf

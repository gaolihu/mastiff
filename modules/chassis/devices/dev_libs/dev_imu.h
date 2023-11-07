/*
 * @Date: 2023-11-06 14:02:19
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/devices/dev_libs/dev_imu.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
#pragma once

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/devices/device_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    class DeviceIMU : public DeviceBaseItf {
        public:
            DeviceIMU(const ChassisConfig*);
            virtual ~DeviceIMU();

        private:
    };

} //namespace device
} //namespace chss
} //namespace mstf

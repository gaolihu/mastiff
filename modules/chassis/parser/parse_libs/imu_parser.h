/*
 * @Date: 2023-11-06 13:56:19
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/parser/parse_libs/imu_parser.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/aventurier_common_msgs/proto/ros_msgs/sensor_msgs.pb.h"
#include "modules/aventurier_common_msgs/proto/ros_msgs/geometry_msgs.pb.h"

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

// 角度分辨率为  0.01 度
static const float g_angle_resolution=100.0;
// 加速度度分辨率为  1mg/s, 也就是 0.001g/s
static const float g_acc_resolution=1000.0;
#pragma pack(1)
struct IMUHeader
{
    uint8 header[2] = {0xAA, 0x00};
};
struct IMUData
{
    int8 index = 0;
    int16 yaw_angle;
    int16 yaw_rate;
    int16 roll_angle;
    int16 pitch_angle;
    int16 acc_x;
    uint8_t reseved = 0x55;
};
struct IMUMsgData
{
    IMUHeader header;
    IMUData data;
    uint8 check_sum = 0;
};
#pragma pack(pop)

    class IMUParser : public ParserBaseItf {
        public:
            IMUParser(const ChassisConfig*,
                    const SensorInfo*);
            virtual  ~IMUParser() final;

            virtual int Init() override;

        private:
            virtual int ParseRawBuffer(const uint8_t* buf,
                    const size_t len) override;
            int ParseSigleFrame(const IMUMsgData*);
            // int TryParseRawData(int, unsigned char*, UartState*, RawData& dat, int&,
            //     int, char*, CmhHeader*, void** )
    };

} //namespace parser
} //namespace chss
} //namespace mstf

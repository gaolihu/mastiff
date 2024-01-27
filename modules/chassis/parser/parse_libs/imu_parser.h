#pragma once

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

// 角度分辨率为  0.01 度
static const float g_angle_resolution=100.0;
// 加速度度分辨率为  1mg/s, 也就是 0.001g/s
static const float g_acc_resolution=1000.0;
#pragma pack(push,1)
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
            IMUParser(const SensorIndicator&);
            virtual  ~IMUParser() final;

            virtual int Init() override;
            virtual int Start() override;
            virtual int Stop() override;
            virtual int Resume() override;
            virtual int Close() override;

        private:
            virtual int ParseUartBuffer(const
                    SensorIndicator*,
                    const uint8_t*,
                    const size_t) override;
            virtual int ParseI2cBuffer(const
                    SensorIndicator*,
                    const uint8_t*,
                    const size_t) override;
            int ParseSigleFrame(const IMUMsgData*);

            int frame_count_ = 0;
    };

} //namespace parser
} //namespace chss
} //namespace mstf

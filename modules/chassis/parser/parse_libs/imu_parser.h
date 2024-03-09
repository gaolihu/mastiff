#pragma once

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    //////////////R6082V//////////////
    // 角度分辨率为  0.01 度
    static const float g_angle_resolution=100.0;
    // 加速度度分辨率为  1mg/s, 也就是 0.001g/s
    static const float g_acc_resolution=1000.0;

#pragma pack(push, 1)
    struct R6082VIMUHeader {
        uint8 header[2] = {0xAA, 0x00};
    };

    struct R6082VIMUData {
        int8 index = 0;
        int16 yaw_angle;
        int16 yaw_rate;
        int16 roll_angle;
        int16 pitch_angle;
        int16 acc_x;
        uint8_t reseved = 0x55;
    };

    struct R6082VIMUMsgData {
        R6082VIMUHeader header;
        R6082VIMUData data;
        uint8 check_sum = 0;
    };
#pragma pack(pop)
    //////////////R6082V//////////////
    ///
    //////////////ZYF143//////////////
#pragma pack(push, 1)
    struct Zyf143IMUData {
        int16_t header = 0xA5A5;

        int16_t acc_x;
        int16_t acc_y;
        int16_t acc_z;

        int16_t ang_x;
        int16_t ang_y;
        int16_t ang_z;

        int16_t yaw;
        int16_t pitch;
        int16_t roll;

        int16_t stamp_us;

        int16_t crc;  //ang_z + yaw + pitch + roll
    };
#pragma pack(pop)
    //////////////ZYF143//////////////

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
            int ParseR6082VFrame(const R6082VIMUMsgData*);
            int ParseZYF143Frame(const Zyf143IMUData*);

            int DevR6082VParse(const SensorIndicator*,
                    const uint8_t*, const size_t);
            int DevZyf143Parse(const SensorIndicator*,
                    const uint8_t*, const size_t);

            std::function<int(const SensorIndicator*,
                    const uint8_t*,
                    const size_t)> dev_parse_ {};

            int frame_count_ = 0;
    };

} //namespace parser
} //namespace chss
} //namespace mstf

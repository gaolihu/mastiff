#pragma once

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class ServoParser : public ParserBaseItf {
        public:
            ServoParser(const SensorIndicator&);
            virtual  ~ServoParser();

            virtual int Init() override;
            virtual int Start() override;
            virtual int Stop() override;
            virtual int Resume() override;
            virtual int Close() override;

            virtual int WriteServoMessage(const
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>&) override;

        private:
            virtual int ParseCanBuffer(const SensorIndicator*,
                    const uint8_t*, const size_t) override;
            virtual int ParseSigleFrame(const
                    std::vector<uint8_t>&,
                    const size_t) override;

        private:
            int32_t left_rpm_ = 0;
            int32_t last_left_rpm_ = 0;
            int32_t right_rpm_ = 0;
            int32_t last_right_rpm_ = 0;
            int32_t left_encoder_ = 0;
            int32_t last_left_encoder_ = 0;
            int32_t right_encoder_ = 0;
            int32_t last_right_encoder_ = 0;

            float pose_x_ = 0;
            float pose_y_ = 0;
            float pose_theta_ = 0;

            int32_t left_status_ = 0;
            int32_t right_status_ = 0;

            int32_t odom_seq_ = 0;
    };

} //namespace parser
} //namespace chss
} //namespace mstf

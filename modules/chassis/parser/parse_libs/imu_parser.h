/*
 * @Date: 2023-11-06 13:56:19
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/parser/parse_libs/imu_parser.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class IMUParser : public ParserBaseItf {
        public:
            IMUParser(const ChassisConfig*,
                    const SensorInfo*);
            virtual  ~IMUParser() final;

            virtual int Init() override;

        private:
            /*
            virtual int ParseRawBuffer() override;
            virtual int ParseSigleFrame(const
                    std::vector<uint8_t>&,
                    const size_t) override;
                    */
    };

} //namespace parser
} //namespace chss
} //namespace mstf

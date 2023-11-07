/*
 * @Date: 2023-11-06 13:56:26
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/parser/parse_libs/imu_parser.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "modules/chassis/parser/parse_libs/imu_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    IMUParser::IMUParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "IMUParser construct";
        }

    IMUParser::~IMUParser() {
        AINFO << "IMUParser de-construct";
    }

    int IMUParser::Init() {
        return ParserBaseItf::Init(chs_conf_->
                mcu_dev().sn_ind().port(),
                &chs_conf_->imu_dev().uart_conf().buf_setting());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

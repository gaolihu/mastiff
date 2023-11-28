/*
 * @Date: 2023-11-02 19:51:43
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/parser/parse_libs/camera_parser.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class CameraParser : public ParserBaseItf {
        public:
            CameraParser(const ChassisConfig*,
                    const SensorInfo*);
            virtual  ~CameraParser() final;

            virtual int Init() override;

        private:
            virtual int ParseSocInfo(const Message&);
    };

} //namespace parser
} //namespace chss
} //namespace mstf

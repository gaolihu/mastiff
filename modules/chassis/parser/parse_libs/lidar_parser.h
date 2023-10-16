#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class LidarParser : public ParserBaseItf {
        public:
            LidarParser(const ChassisConfig*,
                    const SensorInfo*);
            virtual  ~LidarParser() final;

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

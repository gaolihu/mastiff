#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class LineLaserParser : public ParserBaseItf {
        public:
            LineLaserParser(const ChassisConfig*,
                    const SensorInfo*);
            virtual  ~LineLaserParser() final;

            virtual int Init() override;
            virtual int WriteMiscMessage(const
                    DownToMiscData&) override;

        private:
            virtual int ParseRawBuffer(const uint8_t*,
                    const size_t) override;
            virtual int ParseSigleFrame(const
                    std::vector<uint8_t>&,
                    const size_t) override;
    };

} //namespace parser
} //namespace chss
} //namespace mstf

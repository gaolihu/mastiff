#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/frame_down_stream.pb.h"

namespace mstf {
namespace chss {
namespace parser {

    using namespace /*mstf::chss::*/proto;

    class InfoPacker {
        public:
            InfoPacker();
            virtual ~InfoPacker();

            std::vector<uint8_t> PackMessage(const
                    DownToMcuData&);

            std::vector<uint8_t> PackMotorMessage(const
                    DownToMcuData&);

        private:
            uint16_t frame_seq_ = 0;

            uint16_t GenerateCheckSum(uint8_t*, uint32_t);
            uint16_t InsertData(const DownToMcuData&,
                std::vector<uint8_t>&);
    };

} //namespace parser
} //namespace chss
} //namespace mstf

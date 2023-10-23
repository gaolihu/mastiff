#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/frame_down_stream.pb.h"

namespace mstf {
namespace chss {
namespace parser {

    using namespace /*mstf::chss::*/proto;

    class PackerBaseItf {
        public:
            PackerBaseItf(const std::string& dev) {
                AINFO << "PackerBaseItf construct for " << dev;
            }

            virtual ~PackerBaseItf() {
                AINFO << "PackerBaseItf de-construct";
            }

            virtual const std::vector<uint8_t>
            PackMcuMessage(const DownToMcuData&) {
                return {};
            }

            //for servo motor device
            virtual std::tuple<const int, const std::vector<uint8_t>>
            PackMotorMessageRaw(const DownToServoData&) {
                return {};
            }

            virtual std::vector<std::tuple<const int, const std::vector<uint8_t>>>
            PackMotorMessageArrayRaw(const DownToServoData&) {
                return {};
            }

            virtual const std::vector<uint8_t>
            PackMotorMessageString(const DownToServoData&) {
                return {};
            }

            //for serial device
            virtual const std::vector<uint8_t>
            PackLineLaserMessageRaw(const DownToMiscData&) {
                return {};
            }
    };

} //namespace parser
} //namespace chss
} //namespace mstf

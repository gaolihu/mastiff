#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chss_io.pb.h"

namespace mstf {
namespace chss {
namespace parser {

    using namespace /*mstf::chss::*/proto;

    class PackerBaseItf {
        public:
            PackerBaseItf(const std::string& dev) {
#ifdef CHSS_PKG_DBG
                AINFO << "PackerBaseItf construct: " <<
                    dev;
#endif
            }

            virtual ~PackerBaseItf() {
#ifdef CHSS_PKG_DBG
                AINFO << "PackerBaseItf de-construct";
#endif
            }

            virtual const std::vector<uint8_t>
            PackMcuMessage(/*const DownToMcuData&*/) {
                return {};
            }

            ///////// servo motor device /////////
            virtual std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>
            PackServoMtrControl(const ChsMovementCtrl&) {
                AERROR << "shall override control in servo packer!!";
                return {};
            }
            virtual std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>
                        PackServoMtrSetting(const
                                ServoSetting&) {
                AERROR << "shall override setting in servo packer!!";
                return {};
            }
            ///////// servo motor device /////////

            //for serial devices
            //line laser
            virtual const std::vector<uint8_t>
                PackLlaserRawSetting(const
                        LineLaserSetting&) {
                return {};
            }
    };

} //namespace parser
} //namespace chss
} //namespace mstf

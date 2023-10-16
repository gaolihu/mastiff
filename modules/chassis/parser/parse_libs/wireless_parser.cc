#include "modules/chassis/parser/parse_libs/wireless_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    WirelessParser::WirelessParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "WirelessParser construct";
        }

    WirelessParser::~WirelessParser() {
        AINFO << "WirelessParser de-construct";
    }

    int WirelessParser::Init() {
        return ParserBaseItf::Init(chs_conf_->
                wireless_dev().sn_ind().port());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

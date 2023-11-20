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
        auto dev = dynamic_cast<const WirelessDevice&>(GetDevice());
        return ParserBaseItf::Init(dev.sn_ind().port());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

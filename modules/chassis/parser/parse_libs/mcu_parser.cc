#include "modules/chassis/parser/parse_libs/mcu_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    McuParser::McuParser(const ChassisConfig* cc,
            const SensorIndicator* si) :
        ParserBaseItf(cc, si) {
            AINFO << "McuParser construct";
        }

    McuParser::~McuParser() {
        AINFO << "McuParser de-construct";
    }

    int McuParser::Init() {
        return ParserBaseItf::Init(
                dynamic_cast<const McuDevConf&>
                (GetDevConfig()).sn_ind().port());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

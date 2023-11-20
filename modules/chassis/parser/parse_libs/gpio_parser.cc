#include "modules/chassis/parser/parse_libs/gpio_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    GpioParser::GpioParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "GpioParser construct";
        }

    GpioParser::~GpioParser() {
        AINFO << "GpioParser de-construct";
    }

    int GpioParser::Init() {
        auto dev = dynamic_cast<const GpioDevices&>(GetDevice());
        return ParserBaseItf::Init(dev.sn_ind().port());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

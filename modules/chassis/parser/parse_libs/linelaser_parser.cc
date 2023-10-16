#include "modules/chassis/parser/parse_libs/linelaser_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    LineLaserParser::LineLaserParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "LineLaserParser construct";
        }

    LineLaserParser::~LineLaserParser() {
        AINFO << "LineLaserParser de-construct";
    }

    int LineLaserParser::Init() {
        return ParserBaseItf::Init(chs_conf_->
                linelaser_dev().sn_ind().port());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

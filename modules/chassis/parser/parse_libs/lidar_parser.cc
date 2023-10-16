#include "modules/chassis/parser/parse_libs/lidar_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    LidarParser::LidarParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "LidarParser construct";
        }

    LidarParser::~LidarParser() {
        AINFO << "LidarParser de-construct";
    }

    int LidarParser::Init() {
        return ParserBaseItf::Init(chs_conf_->
                lidar_dev().sn_ind().port());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

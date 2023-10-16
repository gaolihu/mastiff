#include "modules/chassis/parser/parse_libs/camera_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    CameraParser::CameraParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "CameraParser construct";
        }

    CameraParser::~CameraParser() {
        AINFO << "CameraParser de-construct";
    }

    int CameraParser::Init() {
        return ParserBaseItf::Init(chs_conf_->
                camera_dev().sn_ind().port());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

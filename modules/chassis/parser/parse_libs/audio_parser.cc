#include "modules/chassis/parser/parse_libs/audio_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    AudioParser::AudioParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "AudioParser construct";
        }

    AudioParser::~AudioParser() {
        AINFO << "AudioParser de-construct";
    }

    int AudioParser::Init() {
        return ParserBaseItf::Init(
                dynamic_cast<const AudioDevConf&>
                (GetDevConfig()).sn_ind().port());
    }

} //namespace parser
} //namespace chss
} //namespace mstf

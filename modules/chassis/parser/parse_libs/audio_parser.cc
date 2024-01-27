#include "modules/chassis/parser/parse_drv_link.h"
#include "modules/chassis/parser/parse_libs/audio_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    AudioParser::AudioParser(const
            SensorIndicator& si) :
        ParserBaseItf(si) {
#ifdef CHSS_PKG_DBG
            AINFO << "AudioParser construct";
#endif
        }

    AudioParser::~AudioParser() {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioParser de-construct";
#endif
    }

    int AudioParser::Init() {
        if (ParserBaseItf::Init() != 0) {
            AERROR << "AudioParser init error!";
            return -1;
        }
        return ParseDrvLink::Instance()->Init(s_idc_);
    }

    int AudioParser::Start() {
        if (ParserBaseItf::Start() != 0) {
            AERROR << "AudioParser start error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Start(s_idc_);
    }

    int AudioParser::Stop() {
        if (ParserBaseItf::Stop() != 0) {
            AERROR << "AudioParser stop error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Stop(s_idc_);
    }

    int AudioParser::Resume() {
        if (ParserBaseItf::Resume() != 0) {
            AERROR << "AudioParser resume error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Resume(s_idc_);
    }

    int AudioParser::Close() {
        if (ParserBaseItf::Close() != 0) {
            AERROR << "AudioParser close error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Close(s_idc_);
    }

} //namespace parser
} //namespace chss
} //namespace mstf

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
#ifdef CHSS_PKG_DBG
        AINFO << "AudioParser init";
#endif
        if (ParserBaseItf::Init() != 0) {
            AERROR << "AudioParser init error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Init(s_idc_);
    }

    int AudioParser::Start() {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioParser start";
#endif
        if (ParserBaseItf::Start() != 0) {
            AERROR << "AudioParser start error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Start(s_idc_);
    }

    int AudioParser::Stop() {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioParser stop";
#endif
        if (ParserBaseItf::Stop() != 0) {
            AERROR << "AudioParser stop error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Stop(s_idc_);
    }

    int AudioParser::Resume() {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioParser resume";
#endif
        if (ParserBaseItf::Resume() != 0) {
            AERROR << "AudioParser resume error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Resume(s_idc_);
    }

    int AudioParser::Close() {
#ifdef CHSS_PKG_DBG
        AINFO << "AudioParser close";
#endif
        if (ParserBaseItf::Close() != 0) {
            AERROR << "AudioParser close error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Close(s_idc_);
    }

    int AudioParser::WriteSocMessage(const Message& m) {
#ifdef CHSS_PKG_DBG
        //AINFO << "write soc message\n" << m.DebugString();
#endif
        return ParseDrvLink::Instance()->WriteSoc(s_idc_, m);
    }

} //namespace parser
} //namespace chss
} //namespace mstf

#include "modules/chassis/parser/parse_drv_link.h"
#include "modules/chassis/parser/parse_libs/wireless_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    WirelessParser::WirelessParser(const
            SensorIndicator& si) :
        ParserBaseItf(si) {
            AINFO << "WirelessParser construct";
        }

    WirelessParser::~WirelessParser() {
        AINFO << "WirelessParser de-construct";
    }

    int WirelessParser::Init() {
        if (ParserBaseItf::Init() != 0) {
            AERROR << "WirelessParser init error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Init(s_idc_);
    }

    int WirelessParser::Start() {
        if (ParserBaseItf::Start() != 0) {
            AERROR << "WirelessParser start error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Start(s_idc_);
    }

    int WirelessParser::Stop() {
        if (ParserBaseItf::Stop() != 0) {
            AERROR << "WirelessParser stop error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Stop(s_idc_);
    }

    int WirelessParser::Resume() {
        if (ParserBaseItf::Resume() != 0) {
            AERROR << "WirelessParser resume error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Resume(s_idc_);
    }

    int WirelessParser::Close() {
        if (ParserBaseItf::Close() != 0) {
            AERROR << "WirelessParser close error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Close(s_idc_);
    }

    int WirelessParser::ParseSocMsg(const
            SensorIndicator* si, const Message& msg) {
        if(frame_processor_){
            //return frame_processor_(&msg);
        }

        return -1;
    }

} //namespace parser
} //namespace chss
} //namespace mstf

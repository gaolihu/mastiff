#include "modules/chassis/parser/parse_drv_link.h"
#include "modules/chassis/parser/parse_libs/camera_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    CameraParser::CameraParser(const
            SensorIndicator& si) :
        ParserBaseItf(si) {
            AINFO << "CameraParser construct";
        }

    CameraParser::~CameraParser() {
        AINFO << "CameraParser de-construct";
    }

    int CameraParser::Init() {
        if (ParserBaseItf::Init() != 0) {
            AERROR << "CameraParser init error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Init(s_idc_);
    }

    int CameraParser::Start() {
        if (ParserBaseItf::Start() != 0) {
            AERROR << "CameraParser start error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Start(s_idc_);
    }

    int CameraParser::Stop() {
        if (ParserBaseItf::Stop() != 0) {
            AERROR << "CameraParser stop error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Stop(s_idc_);
    }

    int CameraParser::Resume() {
        if (ParserBaseItf::Resume() != 0) {
            AERROR << "CameraParser resume error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Resume(s_idc_);
    }

    int CameraParser::Close() {
        if (ParserBaseItf::Close() != 0) {
            AERROR << "CameraParser close error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Close(s_idc_);
    }

    int CameraParser::ParseSocMsg(const
            SensorIndicator* si, const Message& msg) {
        //TODO GLH, 1/27
        //if(frame_processor_){
            //return frame_processor_(&msg);
        //}
        return 0;
    }

} //namespace parser
} //namespace chss
} //namespace mstf

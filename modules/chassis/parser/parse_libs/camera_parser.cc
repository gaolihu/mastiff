#include "modules/chassis/parser/parse_drv_link.h"
#include "modules/chassis/parser/parse_libs/camera_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    CameraParser::CameraParser(const
            SensorIndicator& si) :
        ParserBaseItf(si) {
            AINFO << "CameraParser construct";

            ParseDrvLink::Instance()->RegisterSocListener
                (std::bind(&ParserBaseItf::OnOriginalDataSoc,
                           this, ::_1, ::_2, ::_3, ::_4), s_idc_);
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
        auto cam = dynamic_cast<const CameraPopDatas&>(msg);
        if (cam.has_rgb()) {
            frame_processor_sp_(std::make_shared
                    <ventura::common_msgs::sensor_msgs::Image>(cam.rgb()),
                    CommonItf::Instance()->GetTopic1(s_idc_));
        }
        if (cam.has_depth()) {
            frame_processor_sp_(std::make_shared
                    <ventura::common_msgs::sensor_msgs::Image>(cam.depth()),
                    CommonItf::Instance()->GetTopic2(s_idc_));
        }
        if (cam.has_pcl()) {
            frame_processor_sp_(std::make_shared
                    <ventura::common_msgs::sensor_msgs::PointCloud2>(cam.pcl()),
                    CommonItf::Instance()->GetTopic3(s_idc_));
        }

        return 0;
    }

} //namespace parser
} //namespace chss
} //namespace mstf

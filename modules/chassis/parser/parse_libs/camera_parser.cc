/*
 * @Date: 2023-11-02 19:51:43
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/parser/parse_libs/camera_parser.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
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
        return ParserBaseItf::Init(
                dynamic_cast<const CameraDevConf&>
                (GetDevConfig()).sn_ind().port());
    }
    int CameraParser::ParseSocInfo(const Message& msg)
    {
        ADEBUG << "CameraParser  ParseSocInfo";
        if(frame_processor_){
            auto type = msg.GetTypeName();
            if(type.find("Image") != type.npos){
                return frame_processor_(&msg, "ventura::common_msgs::sensor_msgs::Image");
            }else if(type.find("PointCloud2") != type.npos){
                return frame_processor_(&msg, "ventura::common_msgs::sensor_msgs::PointCloud2");
            }
        }
        return 0;
    }

} //namespace parser
} //namespace chss
} //namespace mstf

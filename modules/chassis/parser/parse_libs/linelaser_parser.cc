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
        return ParserBaseItf::Init(
                dynamic_cast<const LineLaserDevice&>
                (GetDevice()).sn_ind().port(),
                &dynamic_cast<const LineLaserDevice&>
                (GetDevice()).uart_conf().buf_setting());
    }

    int LineLaserParser::WriteMiscMessage(const DownToMiscData& data) {
#if 0
        AINFO << "data to line-laser:\n" << data.DebugString();
#endif
        const std::vector<uint8_t> packed_data =
            packer_->PackLineLaserMessageRaw(data);
        if (RawManage::Instance()->WriteUart(snsr_info_, packed_data) ==
                packed_data.size()) {
            return 0;
        }

        return -1;
    }

    int LineLaserParser::ParseRawBuffer(const uint8_t* buf,
            const size_t len) {
#if 0
        AINFO << "ParseRawBuffer len ~ " <<
            len << ", [" <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[0]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[1]) <<
            " | " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[2]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[3]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[4]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[5]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[6]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[7]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[8]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[9]) <<
            " ]";
#endif
        //if the current frame is good enough to parse
        //a whole frame of data, send upstream directly

        //TODO

        return 0;
    }

    int LineLaserParser::ParseSigleFrame(const
            std::vector<uint8_t>& data,
            const size_t len) {
        //get protobuf frame & frame_processor_(frame);
        //TODO
        //     
        return 0;
    }

} //namespace parser
} //namespace chss
} //namespace mstf

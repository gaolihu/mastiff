#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/servo_parser.h"
#include "modules/chassis/parser/parse_libs/ds20270da_driver.h"

namespace mstf {
namespace chss {
namespace parser {

    ServoParser::ServoParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "ServoParser construct";
        }

    ServoParser::~ServoParser() {
        AINFO << "ServoParser de-construct";
    }

    int ServoParser::Init() {
        return ParserBaseItf::Init(chs_conf_->
                servo_dev().sn_ind().port(),
                &chs_conf_->servo_dev().
                can_conf().buf_setting());
    }

    int ServoParser::WriteServoMessage(const DownToServoData& data) {
#if 1
        AINFO << "data to servo motor:\n" << data.DebugString();
#endif
#if 0
        //for testing
        std::vector<uint8_t> packed_data =
            packer_->PackMotorMessageString(data);
        return RawManage::Instance()->WriteCan(packed_data);
#else
        if (data.has_diff_speed()) {
            std::tuple<const int,
                const std::vector<uint8_t>> packed_data =
                    packer_->PackMotorMessageRaw(data);
            return RawManage::Instance()->
                WriteCan(std::get<0>(packed_data),
                        &(std::get<1>(packed_data))[0],
                        std::get<1>(packed_data).size());
        } else {
            std::vector<std::tuple<const int,
                const std::vector<uint8_t>>> packed_datas =
                    packer_->PackMotorMessageArrayRaw(data);
            AINFO << "send tuple num: " << packed_datas.size();
            for (int i = 0; i < (int)packed_datas.size(); i++) {
                RawManage::Instance()->
                    WriteCan(std::get<0>(packed_datas[i]),
                            &(std::get<1>(packed_datas[i]))[0],
                            std::get<1>(packed_datas[i]).size());
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }

            return 0;
        }
#endif
    }

    int ServoParser::ParseCanBuffer(const uint8_t* buf,
            const size_t len) {
#if 0
        AINFO << "ParseCanBuffer len ~ " <<
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

        parse_servo_motor_info(buf, len);
        //TODO 

        return 0;
    }

    int ServoParser::ParseSigleFrame(const
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

#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/parser/parser_base_itf.h"
#include "modules/chassis/parser/parse_libs/lidar_50cr.h"

namespace mstf {
namespace chss {
namespace parser {

    class LidarParser : public ParserBaseItf {
        public:
            LidarParser(const ChassisConfig*,
                    const SensorInfo*);
            virtual  ~LidarParser() final;

            virtual int Init() override;

        private:
            virtual int ParseRawBuffer(const uint8_t*,
                    const size_t) override;
            virtual int ParseSigleFrame(const
                    std::vector<uint8_t>&,
                    const size_t) override;

            int TryParseRawData(int, unsigned char*,
                    UartState*, RawData& dat, int&,
                    int, char*, CmdHeader*, void**);

        private:
            FanSegment_AA** fan_segs_ = nullptr;
            UserLidarData userdata_ = {};
            ACTION action_ = OFFLINE;
            LidarMsgHdr zonemsg_ = {};
            EEpromV101 eepromv101_ = {};
            CmdHeader cmdheader_;

            int state_ = -1;
            int start_angle_ = -1;
            int error_num_ = 0;

            std::string error_;
            //whole lidar data
            std::vector<RawData> whole_datas_;

            float cut_from_ = 0.f;
            float cut_to_ = 0.f;
            float cut_stuff_ = 1;

            double point_cloud_last_ = 0;
    };

} //namespace parser
} //namespace chss
} //namespace mstf

#include "modules/chassis/proto/chss_io.pb.h"

#include "modules/chassis/parser/pack_libs/llaser_packer.h"
#include "modules/chassis/parser/parse_libs/ydlidar_protocol.h"

namespace mstf {
namespace chss {
namespace parser {

    LlaserPacker::LlaserPacker(const std::string& dev) :
        PackerBaseItf(dev) {
            AINFO << "LlaserPacker construct for " << dev;
        }

    LlaserPacker::~LlaserPacker() {
        AINFO << "LlaserPacker de-construct";
    }

    const std::vector<uint8_t>
    LlaserPacker::PackLlaserRawSetting(const
            LineLaserSetting& data) {
        uint8_t cmd = 0;
        std::vector<uint8_t> d_pack;

        switch (data.dev_manage().sub_opts()) {
            case E_SUBDEV_OPTS_START:
                cmd = GS_LIDAR_CMD_SCAN;
                break;
            case E_SUBDEV_OPTS_STORE:
                cmd = LIDAR_CMD_FORCE_SCAN;
                break;
            case E_SUBDEV_OPTS_STOP:
                cmd = GS_LIDAR_CMD_STOP;
                break;
            case E_SUBDEV_OPTS_RAW:
                cmd = static_cast<uint8_t>
                    (data.dev_manage().cmd());
                break;
            default:
                AWARN << "ignore linelaser data:\n" <<
                    data.DebugString();
                return {};
        }

        d_pack.emplace_back(LIDAR_CMD_SYNC_BYTE);
        d_pack.emplace_back(LIDAR_CMD_SYNC_BYTE);
        d_pack.emplace_back(LIDAR_CMD_SYNC_BYTE);
        d_pack.emplace_back(LIDAR_CMD_SYNC_BYTE);
        d_pack.emplace_back(0);     //address
        d_pack.emplace_back(cmd);   //cmd
        d_pack.emplace_back(0);     //size
        d_pack.emplace_back(0);     //size
        d_pack.emplace_back(cmd);   //checksum

        return d_pack;
    }

} //namespace parser
} //namespace chss
} //namespace mstf

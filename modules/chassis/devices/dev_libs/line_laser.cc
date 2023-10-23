#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/linelaser_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/line_laser.h"

namespace mstf {
namespace chss {
namespace device {

    LineLaser::LineLaser(const ChassisConfig* cc) :
            DeviceBaseItf(cc->linelaser_dev().si(),
                    cc->linelaser_dev().sn_ind()) {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser construct" <<
#if 0
            ":\n{\n" <<
            cc->linelaser_dev().DebugString() << "}";
#else
            "";
#endif
#endif
        data_parser_ = std::make_unique
            <LineLaserParser>(cc, &cc->linelaser_dev().si());
        DataTransact::Instance()->RegisterDevice(
                cc->linelaser_dev().si().name(),
                cc->linelaser_dev().sn_ind(),
                dynamic_cast<DeviceBaseItf*>(this));
    }

    LineLaser::~LineLaser() {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser de-construct";
#endif
    }

    int LineLaser::Init(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser init";
#endif
        DeviceBaseItf::Init();

        DownToMiscData data;
        data.set_opt(E_SUBDEV_OPTS_INIT);

        return data_parser_->WriteMiscMessage(data);
    }

    int LineLaser::Start(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser start";
#endif
        DeviceBaseItf::Start();

        DownToMiscData data;

        data.set_opt(E_SUBDEV_OPTS_STOP);
        data_parser_->WriteMiscMessage(data);
        data.set_opt(E_SUBDEV_OPTS_STOP);
        data_parser_->WriteMiscMessage(data);

        data.set_opt(E_SUBDEV_OPTS_RAW);
        data.set_raw_data(0x6b);
        data_parser_->WriteMiscMessage(data);

        data.set_opt(E_SUBDEV_OPTS_RAW);
        data.set_raw_data(0x62);
        data_parser_->WriteMiscMessage(data);

        data.set_opt(E_SUBDEV_OPTS_STOP);
        data_parser_->WriteMiscMessage(data);

        data.set_opt(E_SUBDEV_OPTS_RAW);
        data.set_raw_data(0x60);
        data_parser_->WriteMiscMessage(data);

        data.set_opt(E_SUBDEV_OPTS_STORE);
        data_parser_->WriteMiscMessage(data);

        data.set_opt(E_SUBDEV_OPTS_START);
        return data_parser_->WriteMiscMessage(data);
    }

    int LineLaser::Stop(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser stop";
#endif
        DownToMiscData data;
        data.set_opt(E_SUBDEV_OPTS_STOP);
        //stop line laser
        data_parser_->WriteMiscMessage(data);

        return DeviceBaseItf::Stop();
    }

} //namespace device
} //namespace chss
} //namespace mstf

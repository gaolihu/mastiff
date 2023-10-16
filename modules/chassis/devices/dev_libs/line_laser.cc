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

} //namespace device
} //namespace chss
} //namespace mstf

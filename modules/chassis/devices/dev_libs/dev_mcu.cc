#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/mcu_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_mcu.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceMcu::DeviceMcu(const ChassisConfig* cc) :
            DeviceBaseItf(cc->mcu_dev().si(),
                    cc->mcu_dev().sn_ind()) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceMcu construct" <<
#if 0
            ":\n{\n" <<
            cc->mcu_dev().DebugString() << "}";
#else
            "";
#endif
#endif

        data_parser_ = std::make_unique
            <McuParser>(cc, &cc->mcu_dev().si());
        DataTransact::Instance()->RegisterDevice(
                cc->mcu_dev().si().name(),
                cc->mcu_dev().sn_ind(),
                dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceMcu::~DeviceMcu() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceMcu de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/imu_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_imu.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceIMU::DeviceIMU(const ChassisConfig* cc) :
            DeviceBaseItf(cc->imu_dev().si(),
                    cc->imu_dev().sn_ind()) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceIMU construct" <<
#if 0
            ":\n{\n" <<
            cc->imu_dev().DebugString() << "}";
#else
            "";
#endif
#endif

        data_parser_ = std::make_unique
            <IMUParser>(cc, &cc->imu_dev().si());
        DataTransact::Instance()->RegisterDevice(
                cc->imu_dev().si().name(),
                cc->imu_dev().sn_ind(),
                dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceIMU::~DeviceIMU() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceIMU de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

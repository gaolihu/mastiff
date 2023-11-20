#include "modules/chassis/parser/parse_libs/imu_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_imu.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceIMU::DeviceIMU(const ChassisConfig* cc,
            const SensorInfo& si,
            const SensorIndicator& idc) :
            DeviceBaseItf(si, idc) {
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
            <IMUParser>(cc, &si);
        DataTransact::Instance()->RegisterDevice(
                si.name(), idc,
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

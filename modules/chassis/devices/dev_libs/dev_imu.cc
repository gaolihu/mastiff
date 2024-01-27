#include "modules/chassis/parser/parse_libs/imu_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/dev_imu.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceIMU::DeviceIMU(const
            SensorIndicator& idc) :
        DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceIMU construct";
#endif
        data_parser_ = std::make_unique
            <IMUParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceIMU::~DeviceIMU() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceIMU de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/mcu_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/dev_mcu.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceMcu::DeviceMcu(const
            SensorIndicator& idc) :
            DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceMcu construct";
#endif
        data_parser_ = std::make_unique
            <McuParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceMcu::~DeviceMcu() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceMcu de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

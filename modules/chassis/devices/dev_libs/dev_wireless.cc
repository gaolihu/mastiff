#include "modules/chassis/parser/parse_libs/wireless_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/dev_wireless.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceWireless::DeviceWireless(const
            SensorIndicator& idc) :
            DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceWireless construct";
#endif
        data_parser_ = std::make_unique
            <WirelessParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceWireless::~DeviceWireless() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceWireless de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

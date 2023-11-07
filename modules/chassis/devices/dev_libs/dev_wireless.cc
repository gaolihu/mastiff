#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/wireless_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_wireless.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceWireless::DeviceWireless(const ChassisConfig* cc) :
            DeviceBaseItf(cc->wireless_dev().si(),
                    cc->wireless_dev().sn_ind()) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceWireless construct" <<
#if 0
            ":\n{\n" <<
            cc->wireless_dev().DebugString() << "}";
#else
            "";
#endif
#endif
        data_parser_ = std::make_unique
            <WirelessParser>(cc, &cc->wireless_dev().si());
        DataTransact::Instance()->RegisterDevice(
                cc->wireless_dev().si().name(),
                cc->wireless_dev().sn_ind(),
                dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceWireless::~DeviceWireless() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceWireless de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

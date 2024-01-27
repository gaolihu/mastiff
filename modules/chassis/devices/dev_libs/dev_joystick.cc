#include "modules/chassis/parser/parse_libs/joystick_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/dev_joystick.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceJoyStick::DeviceJoyStick(const
            SensorIndicator& idc) :
            DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceJoyStick construct";
#endif
        data_parser_ = std::make_unique
            <JoyStickParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceJoyStick::~DeviceJoyStick() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceJoyStick de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

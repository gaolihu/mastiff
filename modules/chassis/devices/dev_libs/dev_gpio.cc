#include "modules/chassis/parser/parse_libs/gpio_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/dev_gpio.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceGpio::DeviceGpio(const
            SensorIndicator& idc) :
            DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceGpio construct";
#endif
        data_parser_ = std::make_unique
            <GpioParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceGpio::~DeviceGpio() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceGpio de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

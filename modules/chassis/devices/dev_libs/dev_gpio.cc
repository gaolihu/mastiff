#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/gpio_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_gpio.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceGpio::DeviceGpio(const ChassisConfig* cc,
            const SensorInfo& si,
            const SensorIndicator& idc) :
            DeviceBaseItf(si, idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceGpio construct" <<
#if 0
            ":\n{\n" <<
            cc->gpio_dev().DebugString() << "}";
#else
            "";
#endif
#endif
        data_parser_ = std::make_unique
            <GpioParser>(cc, &si);
        DataTransact::Instance()->RegisterDevice(
                si.name(), idc,
                dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceGpio::~DeviceGpio() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceGpio de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

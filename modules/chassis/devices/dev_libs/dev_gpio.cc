#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/gpio_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_gpio.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceGpio::DeviceGpio(const ChassisConfig* cc) :
            DeviceBaseItf(cc->gpio_dev().si(),
                    cc->gpio_dev().sn_ind()) {
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
            <GpioParser>(cc, &cc->gpio_dev().si());
        DataTransact::Instance()->RegisterDevice(
                cc->gpio_dev().si().name(),
                cc->gpio_dev().sn_ind(),
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

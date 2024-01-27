#include "modules/chassis/parser/parse_libs/lidar_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/dev_lidar.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceLidar::DeviceLidar(const
            SensorIndicator& idc) :
            DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceLidar construct";
#endif
        data_parser_ = std::make_unique
            <LidarParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceLidar::~DeviceLidar() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceLidar de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

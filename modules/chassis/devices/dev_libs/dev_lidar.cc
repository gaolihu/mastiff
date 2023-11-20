#include "modules/chassis/parser/parse_libs/lidar_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_lidar.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceLidar::DeviceLidar(const ChassisConfig* cc,
            const SensorInfo& si,
            const SensorIndicator& idc) :
            DeviceBaseItf(si, idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceLidar construct" <<
#if 0
            ":\n{\n" <<
            cc->lidar_dev().DebugString() << "}";
#else
            "";
#endif
#endif
        data_parser_ = std::make_unique
            <LidarParser>(cc, &si);
        DataTransact::Instance()->RegisterDevice(
                si.name(), idc,
                dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceLidar::~DeviceLidar() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceLidar de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

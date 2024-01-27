#include "modules/chassis/parser/parse_libs/camera_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/dev_camera.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceCamera::DeviceCamera(const
            SensorIndicator& idc) :
            DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceCamera construct";
#endif
        data_parser_ = std::make_unique
            <CameraParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceCamera::~DeviceCamera() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceCamera de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

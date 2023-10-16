#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/camera_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_camera.h"

namespace mstf {
namespace chss {
namespace device {

    DeviceCamera::DeviceCamera(const ChassisConfig* cc) :
            DeviceBaseItf(cc->camera_dev().si(),
                    cc->camera_dev().sn_ind()) {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceCamera construct" <<
#if 0
            ":\n{\n" <<
            cc->camera_dev().DebugString() << "}";
#else
            "";
#endif
#endif

        data_parser_ = std::make_unique
            <CameraParser>(cc, &cc->camera_dev().si());
        DataTransact::Instance()->RegisterDevice(
                cc->camera_dev().si().name(),
                cc->camera_dev().sn_ind(),
                dynamic_cast<DeviceBaseItf*>(this));
    }

    DeviceCamera::~DeviceCamera() {
#ifdef CHSS_PKG_DBG
        AINFO << "DeviceCamera de-construct";
#endif
    }

} //namespace device
} //namespace chss
} //namespace mstf

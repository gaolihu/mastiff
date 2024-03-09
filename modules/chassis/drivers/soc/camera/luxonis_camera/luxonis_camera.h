#pragma once

#include "modules/chassis/drivers/soc/camera/camera_itf.h"

namespace mstf {
namespace chss {
namespace driver {

    class LuxonisCamera : public CameraItf {
        public:
            LuxonisCamera(const std::string& dev) {
                dev_ = dev;
            }
            ~LuxonisCamera() {}

            virtual int Init() override;
            virtual int Start() override;
            virtual int Stop() override;
            virtual int Resume() override;
            virtual int Close() override;

            virtual int PollingCameraRutine(CameraPopDatas&) override;

        private:
            std::string dev_;

            size_t frame_seq_ = 0;
    };

}  // namespace driver
}  // namespace chss
}  // namespace mstf

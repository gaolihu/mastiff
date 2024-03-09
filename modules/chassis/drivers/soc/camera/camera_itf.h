#pragma once

#include "modules/chassis/proto/chss_io.pb.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace proto;
    using namespace google::protobuf;

    class CameraItf {
        public:
            virtual int Init() = 0;
            virtual int Start() = 0;
            virtual int Stop() = 0;
            virtual int Resume() = 0;
            virtual int Close() = 0;
            //loop and send out camera images
            virtual int PollingCameraRutine(CameraPopDatas&) = 0;
            virtual int SaveImage(const std::string&) {
                return -1;
            };

            virtual int SetCameraCtrl(const CameraSetting&) {
                return -1;
            }
            virtual int SetCameraCtrl(const
                    RepeatedPtrField<CameraSetting>&) {
                return -1;
            }
    };

} //namespace driver
} //namespace chas
} //namespace mstf

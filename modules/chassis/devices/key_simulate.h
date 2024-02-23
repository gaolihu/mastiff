#pragma once

#include "modules/chassis/devices/dev_libs/dev_tty.h"

namespace mstf {
namespace chss {
namespace device {

    using namespace google::protobuf;
    using namespace /*mstf::chss::*/proto;

    using SimulateProtoHandle = std::function<int
        (const std::shared_ptr<Message>&)>;

    class KeySimulate {
        public:
            KeySimulate();
            virtual ~KeySimulate();

            void StartSimulate();
            void RegisterHandle(const SimulateProtoHandle& s) {
                upper_handler_ = s;
            }

        private:
            SimulateProtoHandle upper_handler_ = nullptr;

            //audio
            void AudioPlay(std::shared_ptr<ImportChassisCtrl>&, const int);
            void AudioCtrl(std::shared_ptr<ImportChassisCtrl>&, const int);

            void DeviceManageSim(std::shared_ptr<ImportChassisCtrl>&, const int);

            void WirelessKeySim(std::shared_ptr<ImportChassisCtrl>&, const int);
            void CameraKeySim(std::shared_ptr<ImportChassisCtrl>&, const int);
    };


} // namespace device
} // namespace chss
} // namespace mstf

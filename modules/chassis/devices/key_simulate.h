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

            void DeviceManageSim(std::shared_ptr<ChassisCtrl>&, const int);

            void WirelessKeySim(std::shared_ptr<ChassisCtrl>&, const int);
            void AudioKeySim(std::shared_ptr<ChassisCtrl>&, const int);
    };


} // namespace device
} // namespace chss
} // namespace mstf

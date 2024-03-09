#pragma once

#include "modules/chassis/proto/chss_io.pb.h"
#include "modules/cambrian/proto/cambrian_data.pb.h"

#include "modules/chassis/devices/dev_libs/dev_tty.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace google::protobuf;
    using namespace mstf::chss::proto;
    using namespace /*mstf::camb::*/proto;

    using SimulateProtoHandle = std::function<int
        (const std::shared_ptr<Message>&)>;

    class KeySimulate {
        public:
            KeySimulate();
            virtual ~KeySimulate();

            void StartSimulate();
            void RegisterHandle(const
                    SimulateProtoHandle& s) {
                upper_handler_ = s;
            }

        private:
            //chassis control
            void AudioPlay(const int);
            void AudioCtrl(const int);
            void DeviceManage(const int);
            void WirelessSetting(const int);
            void CameraSetting(const int);
            //chassis control
            void MissionSetting(const int);

            SimulateProtoHandle upper_handler_ = nullptr;

            std::shared_ptr<MiscChassisCtrl> chassis_ctrl_ {};
            std::shared_ptr<MissionCommand> mission_setting_ {};
            //query TODO
    };


} // namespace brain
} // namespace camb
} // namespace mstf

#pragma once

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/input_output_chs.pb.h"

#include "modules/chassis/devices/device_manager.h"
#include "modules/chassis/devices/msg_transfer.h"

namespace mstf {
namespace chss {

    using namespace apollo;
    using namespace /*mstf::chss::*/device;
    using namespace /*mstf::chss::*/proto;

    class ReceiveCtrl {
        public:
            ReceiveCtrl(const std::shared_ptr<ChassisConfig>& cc) {
#ifdef CHSS_PKG_DBG
                AINFO << "ReceiveCtrl construct <MESSAGE DOWNSTREAM>";
#endif
                chs_conf_ = cc;

                subscribe_node_ = cyber::CreateNode(chs_conf_->
                        chs_topic_conf().chassis_input_channel());

                //AINFO << "start to generate devices manager";
                devices_mgr_ = std::make_unique<DeviceManager>(chs_conf_);

                //AINFO << "start to initialize chassis devices";
                devices_mgr_->DeviceInit();

#if 1   //for testing
                AINFO << "start to powerup peripheral devices, for testing";
                devices_mgr_->DeviceStart();
#endif
                AINFO << "build chassis peripheral reader channel";
                auto chs_reader_ = subscribe_node_->CreateReader<ChassisCtrl>(
                        chs_conf_->chs_topic_conf().input_crtl_topic_name(),
                        [&](const std::shared_ptr<ChassisCtrl>& msg)->void {
#ifdef CHSS_PKG_DBG
                            AINFO << "Received chassis ctrl:\n" <<
                                msg.get()->DebugString();
#endif
                            //control chassis in any case
                            ControlChassis(msg);
                        }
                );

                AINFO << "build speed control reader channel";
                auto ctl_reader_ = subscribe_node_->
                    CreateReader<ventura::common_msgs::geometry_msgs::Twist>(
                        chs_conf_->chs_topic_conf().input_move_topic_name(),
                        [&](const std::shared_ptr
                            <ventura::common_msgs::geometry_msgs::Twist>&
                            msg)->void {
                            MsgTransfer::Instance()->
                            TransferChassisMovement(msg);
                        }
                );
#if 1
                //for debuging
                MsgTransfer::Instance()->SetChassisContrlSimItf([&]
                        (const std::shared_ptr<Message>& m)->int {
                            return ControlChassis(std::dynamic_pointer_cast
                                    <ChassisCtrl>(m));
                        });
#endif
            }

            virtual ~ReceiveCtrl() final {
                devices_mgr_->DeviceClose();
#ifdef CHSS_PKG_DBG
                AINFO << "ReceiveCtrl de-construct";
#endif
                MsgTransfer::Instance()->Finish();
            }

            bool ControlChassis(const std::shared_ptr
                    <ChassisCtrl>& ctrl) const {
                //check if need manage peripheral devices
                if (ctrl->has_soc_ctrl())
                    if (ctrl->soc_ctrl().has_dev_maganger()) {
                        devices_mgr_->ManageDevices(
                                ctrl->soc_ctrl().dev_maganger());
                        return true;
                    }

                return MsgTransfer::Instance()->
                    TransferChassisControl(ctrl);
            }
        private:
            std::shared_ptr<ChassisConfig> chs_conf_  = nullptr;

            std::unique_ptr<DeviceManager> devices_mgr_ = nullptr;

            std::unique_ptr<cyber::Node> subscribe_node_ = nullptr;
    };

} //namespace chss
} //namespace mstf

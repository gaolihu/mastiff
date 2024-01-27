#pragma once

#ifndef KEY_SIMULATE
#define KEY_SIMULATE 1
#endif

#ifdef RLS_DATA_PEEP
#define RLS_DATA_PEEP 0
#endif

#include "cyber/common/log.h"
#include "cyber/common/util.h"
#include "cyber/common/macros.h"
#include "cyber/message/message_traits.h"

#include "modules/chassis/proto/chss_io.pb.h"

#ifdef KEY_SIMULATE
#include "modules/chassis/devices/key_simulate.h"
#endif
#include "modules/chassis/parser/common_inf.h"

namespace mstf {
namespace chss {
namespace device {

    using namespace /*mstf::chss::*/proto;
    using namespace google::protobuf;
    using namespace apollo;

    using Ctrl_Movement = std::function<int(const ChsMovementCtrl&)>;
    using Ctrl_PeriphAdc = std::function<int(const ChsPeriphAdcCtrl&)>;
    using Ctrl_PeriphPwm = std::function<int(const ChsPeriphPwmCtrl&)>;
    using Ctrl_PeriphGpio = std::function<int(const ChsPeriphGpioCtrl&)>;
    using Ctrl_PeriphInfra = std::function<int(const ChsPeriphInfraCtrl&)>;
    using Ctrl_SocMisc = std::function<int(const ChsSocMiscCtrl&)>;
    using Ctrl_ChsUpdate = std::function<int(const ChsFirmWareUpdate&)>;

    class Transactor {
        public:
            DECLARE_SINGLETON(Transactor)
            virtual ~Transactor();

        public:
            void  Finish() {
                AINFO << "transactor finish";
                delete this;
            }

            inline void  Init(const std::shared_ptr<
                    ChassisConfig>& cc) {
                AINFO << "transactor init";
                parser::CommonItf::Instance()->
                    SetChasisConfig(cc);
            }

            //1, Dispatch msgs out
            template <typename MessageT>
            void RegisterPublishers(const
                    std::function<int(const
                        std::shared_ptr<Message>&)>& p) {
#ifdef CHSS_PKG_DBG
                AINFO << "TRANS_W[" << msg_publisher_pair_.size() <<
                    "] register msg publisher \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                msg_publisher_pair_[
                    cyber::message::GetMessageName<MessageT>()] = p;
            }

            int OnTransferMessageOut(const std::shared_ptr<Message>& msg) {
#if 0
//#ifdef CHSS_PKG_DBG
                static int i = 0;
                if (i++ == 2) {
                AINFO << "transact output " << msg->GetTypeName() <<
#if 1
                    "\n" << msg->DebugString();
#else
                    "";
#endif
                }
#endif
                for (auto x : msg_publisher_pair_) {
                    if (x.first == msg->GetTypeName()) {
                        //=&MsgsSendTmpl::MessagePublish()
                        return x.second(msg);
                    }
                }

                AERROR << "Transfer out message NG! " <<
                    msg->GetTypeName();

                return -1;
            }

            //2, subscribe msgs & handle
            template <typename MessageT>
            void RegisterReceiver() {
#ifdef CHSS_PKG_DBG
                AINFO << "TRANS_R[" << msgs_receiver_pair_.size() <<
                    "]" << "register msg receiver \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                if (cyber::message::GetMessageName<MessageT>().
                        find("ImportChassisCtrl") != std::string::npos) {
                    msgs_receiver_pair_[
                        cyber::message::GetMessageName<MessageT>()] =
                            [&](const std::shared_ptr<Message>& msg)->int {
                                //forward msg
                                return RecvChassCtrl(
                                        std::dynamic_pointer_cast
                                        <ImportChassisCtrl>(msg));
                            };
                } else if (cyber::message::GetMessageName<MessageT>().
                        find("Twist") != std::string::npos) {
                    msgs_receiver_pair_[
                        cyber::message::GetMessageName<MessageT>()] =
                            [&](const std::shared_ptr<Message>& msg)->int {
                                //forward msg
                                return RecvChassMove(
                                        std::dynamic_pointer_cast
                                        <ventura::common_msgs::
                                        geometry_msgs::Twist>(msg));
                            };
                }
            }

            int OnTransferMessageIn(const
                    std::shared_ptr<Message>& msg) {
#ifdef CHSS_PKG_DBG
                AINFO << "transact input " << msg->GetTypeName() <<
                    "\n" << msg->DebugString();
#endif
                auto it = msgs_receiver_pair_.find(msg->GetTypeName());
                if (it != msgs_receiver_pair_.end()) {
                    return it->second(msg);
                }

                AERROR << "Transfer in message NG! " <<
                    msg->GetTypeName();

                return -1;
            }

            inline void SetMoveExecutor(const Ctrl_Movement& c) {
                ctrl_move_ = c;
            }

            inline void SetAdcExecutor(const Ctrl_PeriphAdc& c) {
                ctrl_adc_ = c;
            }

            inline void SetPwmExecutor(const Ctrl_PeriphPwm& c) {
                ctrl_pwm_ = c;
            }

            inline void SetGpioExecutor(const Ctrl_PeriphGpio& c) {
                ctrl_gpio_ = c;
            }

            inline void SetInfraExecutor(const Ctrl_PeriphInfra& c) {
                ctrl_infra_ = c;
            }

            inline void SetSocExecutor(const Ctrl_SocMisc& c) {
                ctrl_soc_ = c;
            }

            inline void SetUpdateExecutor(const Ctrl_ChsUpdate& c) {
                ctrl_update = c;
            }

        private:
            int RecvChassCtrl(const std::shared_ptr
                    <ImportChassisCtrl>&);
            int RecvChassMove(const std::shared_ptr<
                    ventura::common_msgs::geometry_msgs::Twist>&);

            //specified performer
            inline int _CtrlChsMovement(const
                    ChsMovementCtrl& ctrl) {
                return ctrl_move_(ctrl);
            }

            inline int _CtrlChsPeriphAdc(const
                    ChsPeriphAdcCtrl& ctrl) {
                return ctrl_adc_(ctrl);
            }

            inline int _CtrlChsPeriphPwm(const
                    ChsPeriphPwmCtrl& ctrl) {
                return ctrl_pwm_(ctrl);
            }

            inline int _CtrlChsPeriphGpio(const
                    ChsPeriphGpioCtrl& ctrl) {
                return ctrl_gpio_(ctrl);
            }

            inline int _CtrlChsPeriphInfra(const
                    ChsPeriphInfraCtrl& ctrl) {
                return ctrl_infra_(ctrl);
            }

            inline int _CtrlChsSocMisc(const
                    ChsSocMiscCtrl& ctrl) {
                return ctrl_soc_(ctrl);
            }

            inline int _CtrlChsUpdate(const
                    ChsFirmWareUpdate& ctrl) {
                return ctrl_update(ctrl);
            }

        private:
            using TypeFunction = std::function
                <int(const std::shared_ptr<Message>&)>;
            //publishing
            std::unordered_map<std::string,
                TypeFunction> msg_publisher_pair_ {};
            //subscribing
            std::unordered_map<std::string,
                TypeFunction> msgs_receiver_pair_ {};

#ifdef KEY_SIMULATE
            std::shared_ptr<KeySimulate> key_sim_ = nullptr;
#endif

            Ctrl_Movement ctrl_move_;
            Ctrl_PeriphAdc ctrl_adc_;
            Ctrl_PeriphPwm ctrl_pwm_;
            Ctrl_PeriphGpio ctrl_gpio_;
            Ctrl_PeriphInfra ctrl_infra_;
            Ctrl_SocMisc ctrl_soc_;
            Ctrl_ChsUpdate ctrl_update;
    };

} //namespace device
} //namespace chss
} //namespace mstf

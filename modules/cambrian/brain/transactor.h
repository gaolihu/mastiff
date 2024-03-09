#pragma once

#include "cyber/common/log.h"
#include "cyber/common/macros.h"

#include "modules/cambrian/brain/abstract_bot.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace apollo;
    using namespace /*mstf::camb::*/proto;
    using namespace google::protobuf;

    class Transactor {
        public:
            DECLARE_SINGLETON(Transactor)
            virtual ~Transactor();

        public:
            void  Finish() {
#ifdef CAMB_PKG_DBG
                AINFO << "singleton transactor finish";
#endif
                //AINFO << "Transactor stop robot!";
                abstract_robot_->Stop();
                //AINFO << "Transactor close robot!";
                abstract_robot_->Close();

                delete this;
            }

            void  Init(const std::shared_ptr<CambrianConfig> cc) {
                AINFO << "Transactor init, setting robot";
                if (!abstract_robot_) {
                    abstract_robot_ = std::make_unique<AbstractBot>(cc);
                    abstract_robot_->Init(std::bind(
                                &Transactor::OnTransferMessageOut,
                                this, std::placeholders::_1));
                    abstract_robot_->Start();
                }
            }

            //1, Dispatch
            template <typename MessageT>
            void RegisterPublishers(const
                    std::function<int(const std::shared_ptr<Message>&)>& p) {
#ifdef CAMB_PKG_DBG
                AINFO << "W[" << msg_publisher_pair_.size() <<
                    "] register msg publisher \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                msg_publisher_pair_[
                    cyber::message::GetMessageName<MessageT>()] = p;
            }

            int OnTransferMessageOut(const std::shared_ptr<Message>& msg) {
#if 0
//#ifdef CAMB_PKG_DBG
                AINFO << "transact output " << msg->GetTypeName() <<
                    "\n" << msg->DebugString();
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

            //2, subscribe
            template <typename MessageT>
            void RegisterReceiver() {
#ifdef CAMB_PKG_DBG
                AINFO << "R[" << msgs_receiver_pair_.size() <<
                    "]" << "register msg receiver \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                msgs_receiver_pair_[
                    cyber::message::GetMessageName<MessageT>()] =
                        [&](const std::shared_ptr<Message>& msg)->int {
                            //forward msg
                            return OnTranferMsgInCall<MessageT>
                                (std::dynamic_pointer_cast<MessageT>(msg));
                        };
            }

            template <typename MessageT>
            int OnTranferMsgInCall(const
                    std::shared_ptr<MessageT>& msg) {
                return abstract_robot_->
                    BotItfImpl::MsgsInpouring<MessageT>(msg);
            }

            int OnTransferMessageIn(const std::shared_ptr<Message>& msg) {
#if 0
//#ifdef CAMB_PKG_DBG
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

        private:
            using TypeFunction = std::function
                <int(const std::shared_ptr<Message>&)>;

            //publishing
            std::unordered_map<std::string,
                TypeFunction> msg_publisher_pair_ {};

            //subscribing
            std::unordered_map<std::string,
                TypeFunction> msgs_receiver_pair_ {};

            //robot
            std::unique_ptr<AbstractBot> abstract_robot_;
    };

    Transactor::Transactor() {
#ifdef CAMB_PKG_DBG
        AINFO << "Transactor singleton construct";
#endif
    }

    Transactor::~Transactor() {
#ifdef CAMB_PKG_DBG
        AINFO << "Transactor singleton de-construct";
#endif
        msg_publisher_pair_.clear();
        msgs_receiver_pair_.clear();
    }

} //namespace brain
} //namespace camb
} //namespace mstf

#pragma once

#include "cyber/cyber.h"
#include "cyber/message/message_traits.h"

#include "modules/cambrian/proto/cambrian_arch.pb.h"
#include "modules/chassis/proto/external_msg.pb.h"

#include "modules/cambrian/brain/msgs_send_tmpl.h"

namespace mstf {
namespace camb {

    using namespace apollo;
    using namespace /*mstf::camb::*/proto;
    using namespace google::protobuf;
    using namespace brain;

    class DispatchMsg {
        public:
            DispatchMsg(const std::shared_ptr<CambrianConfig> cc) {
                AINFO << "DispatchMsg construct <MESSAGE OUT>";
#ifdef CAMB_PKG_DBG
                AINFO << "writer node: \"" <<
                    cc->cmb_topic_conf().
                    writer_node_name() << "\"";
#endif
                cc_ = cc;
                publish_node_ = cyber::CreateNode(cc_.lock()->
                        cmb_topic_conf().writer_node_name());
#if 1
                if (cc_.lock()->cmb_topic_conf().has_input_slam_grid_name()) {
                    //test publishing chassis info
                    DispatcherGenerate<ChassisMiscInfo>(
                            cc_.lock()->cmb_topic_conf().
                            input_chs_misc_name().value());
                }
#endif
                if (cc_.lock()->cmb_topic_conf().has_output_chs_topic_name()) {
                    //setting chassis
                    DispatcherGenerate<MiscChassisCtrl>(
                            cc_.lock()->cmb_topic_conf().
                            output_chs_topic_name().value());
                }
            }

            inline void DispatchInit() {
#ifdef CAMB_PKG_DBG
                AINFO << "DispatchInit";
#endif
                Transactor::Instance()->Init(cc_.lock());
            }

            virtual ~DispatchMsg() final {
#ifdef CAMB_PKG_DBG
                AINFO << "DispatchMsg de-construct!";
#endif
                Transactor::Instance()->Finish();
                DispatchChannelDestroy();
            }

        private:
            template <typename MessageT>
            void DispatcherGenerate(const std::string&);
            template <typename MessageT>
            void DispatchChannelBuild();
            void DispatchChannelBuild();
            template <typename MessageT>
            void DispatchChannelDestroy();
            void DispatchChannelDestroy();
            template <typename MessageT>
            int OnMessageDispatch(const std::shared_ptr<MessageT>&);

        private:
            std::unique_ptr<cyber::Node> publish_node_ = nullptr;

            //message & writer-creater pair
            std::map<const std::string,
                std::shared_ptr<MsgsImplItf>
                    > msg_writer_manager_pair_ {};

            std::weak_ptr<CambrianConfig> cc_ {};
    };

    template <typename MessageT> void
    DispatchMsg::DispatcherGenerate(const std::string& msg_name) {
#ifdef CAMB_PKG_DBG
        AINFO << "dispatch channel \"" << msg_name << "\"";
#endif
        AINFO << "D[" <<
            msg_writer_manager_pair_.size() <<
            "] \"" << cyber::message::GetMessageName<MessageT>() << "\"";

        msg_writer_manager_pair_[
            cyber::message::GetMessageName<MessageT>()] =
                std::make_shared<MsgsSendTmpl<MessageT>>
                ([&]()->std::shared_ptr<cyber::Writer<MessageT>> {
                    //create writer
#ifdef CAMB_PKG_DBG
                    AINFO << "creater writer " << "\"" <<
                        cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                    return publish_node_->CreateWriter<MessageT>(msg_name);
                });
        DispatchChannelBuild<MessageT>();
    }

    template <typename MessageT> void
    DispatchMsg::DispatchChannelBuild() {
#ifdef CAMB_PKG_DBG
        AINFO << "build dispatcher \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_writer_manager_pair_) {
            if (x.first == cyber::message::GetMessageName<MessageT>())
                x.second->MsgsFlowoutControl(true);
        }
    }

    void DispatchMsg::DispatchChannelBuild() {
#ifdef CAMB_PKG_DBG
        AINFO << "build dispatcher all";
#endif
        for (auto x : msg_writer_manager_pair_) {
            x.second->MsgsFlowoutControl(true);
        }
    }

    template <typename MessageT> void
    DispatchMsg::DispatchChannelDestroy() {
#ifdef CAMB_PKG_DBG
        AINFO << "destroy dispatcher \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_writer_manager_pair_) {
            if (x.first == cyber::message::GetMessageName<MessageT>())
                x.second->MsgsFlowoutControl(false);
        }
    }

    void DispatchMsg::DispatchChannelDestroy() {
#ifdef CAMB_PKG_DBG
        AINFO << "destroy dispatcher all";
#endif
        for (auto x : msg_writer_manager_pair_) {
            x.second->MsgsFlowoutControl(false);
        }
    }

    template <typename MessageT> int
    DispatchMsg::OnMessageDispatch(const
            std::shared_ptr<MessageT>& msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "dispatch message \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_writer_manager_pair_) {
            if (x.first == cyber::message::GetMessageName<MessageT>()) {
                x.second->MessagePublish(msg);
            }
        }
        return 0;
    }

} //namespace camb
} //namespace mstf

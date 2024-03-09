#pragma once

#include "modules/chassis/devices/msgs_recv_tmpl.h"

namespace mstf {
namespace chss {

    using namespace apollo;
    using namespace /*mstf::chss::*/proto;
    using namespace /*mstf::chss::*/device;

    class ReceiveMsg {
        public:
            ReceiveMsg(const std::shared_ptr<ChassisConfig> cc) {
                AINFO << "ReceiveMsg construct <MESSAGE IN>";
                subscribe_node_ = cyber::CreateNode(cc->
                        chs_topic_conf().chassis_input_channel());
#ifdef CHSS_PKG_DBG
                AINFO << "reader node: \"" <<
                    cc->chs_topic_conf().chassis_input_channel() << "\"";
#endif
                //chassis control
                ReceiverGenerate<MiscChassisCtrl>(
                        cc->chs_topic_conf().
                        input_crtl_topic_name());
                //speed control
                ReceiverGenerate<ventura::common_msgs::geometry_msgs::Twist>(
                        cc->chs_topic_conf().
                        input_move_topic_name());

#if 0
                ReceiverGenerate</*ventura::common_msgs::geometry_msgs::*/PointCloud2>(
                        cc->chs_topic_conf().
                        input_move_topic_name());
#endif
            }

            virtual ~ReceiveMsg() final {
#ifdef CHSS_PKG_DBG
                AINFO << "ReceiveMsg de-construct";
#endif
                ReceiveChannelDestroy();
            }

        private:
            template <typename MessageT>
            void ReceiverGenerate(const std::string&);
            template <typename MessageT>
            void ReceiveChannelBuild();
            void ReceiveChannelBuild();
            template <typename MessageT>
            void ReceiveChannelDestroy();
            void ReceiveChannelDestroy();
            template <typename MessageT>
            int OnMessageReceive(const std::shared_ptr<MessageT>&);

        private:
            std::unique_ptr<cyber::Node> subscribe_node_ = nullptr;

            //message & reader-creater pair
            std::map<const std::string,
                std::shared_ptr<MsgsImplItf>
                    > msg_reader_manager_pair_ {};
    };

    template <typename MessageT> void
    ReceiveMsg::ReceiverGenerate(const std::string& msg_name) {
#ifdef CHSS_PKG_DBG
        AWARN << "build recive channel: \"" << msg_name << "\"";
#endif
        AINFO << "SUB[" <<
            msg_reader_manager_pair_.size() <<
            "] \"" << cyber::message::GetMessageName<MessageT>() << "\"";

        msg_reader_manager_pair_[
            cyber::message::GetMessageName<MessageT>()] =
                std::make_shared<MsgsRcvTmpl<MessageT>>
                ([&]()->std::shared_ptr<cyber::Reader<MessageT>> {
                    //create writer
#ifdef CHSS_PKG_DBG
                    AINFO << "creater reader " << "\"" <<
                        cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                    return subscribe_node_->CreateReader<MessageT>(msg_name,
                            [&](const std::shared_ptr<MessageT>& msg)->int {
                                //OnMessageReceive<MessageT>(std::forward<MessageT>(msg));
                                return OnMessageReceive<MessageT>(msg);
                            });
                            //std::bind(&ReceiveMsg::OnMessageReceive, this,
                                //std::placeholders::_1);
                });
        ReceiveChannelBuild<MessageT>();
    }

    template <typename MessageT> void
    ReceiveMsg::ReceiveChannelBuild() {
#ifdef CHSS_PKG_DBG
        AINFO << "build receiver \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_reader_manager_pair_) {
            if (x.first == cyber::message::GetMessageName<MessageT>())
                x.second->MsgsFlowoutControl(true);
        }
    }

    void ReceiveMsg::ReceiveChannelBuild() {
#ifdef CHSS_PKG_DBG
        AINFO << "build receiver all";
#endif
        for (auto x : msg_reader_manager_pair_) {
            x.second->MsgsFlowoutControl(true);
        }
    }

    template <typename MessageT> void
    ReceiveMsg::ReceiveChannelDestroy() {
#ifdef CHSS_PKG_DBG
        AINFO << "destroy receiver \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_reader_manager_pair_) {
            if (x.first == cyber::message::GetMessageName<MessageT>())
                x.second->MsgsFlowoutControl(false);
        }
    }

    void ReceiveMsg::ReceiveChannelDestroy() {
#ifdef CHSS_PKG_DBG
        AINFO << "destroy receiver all";
#endif
        for (auto x : msg_reader_manager_pair_) {
            x.second->MsgsFlowoutControl(false);
        }
    }

    template <typename MessageT> int
    ReceiveMsg::OnMessageReceive(const
            std::shared_ptr<MessageT>& msg) {
#if 0
//#ifdef CHSS_PKG_DBG
        AINFO << "receive message \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_reader_manager_pair_) {
            if (x.first == cyber::message::GetMessageName<MessageT>()) {
                return x.second->MessageReceive(msg);
            }
        }

        AWARN << "receive message error \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";

        return -1;
    }

} //namespace chss
} //namespace mstf

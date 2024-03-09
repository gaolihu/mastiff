#pragma once

#include "cyber/cyber.h"
#include "cyber/message/message_traits.h"

#include "modules/cambrian/proto/cambrian_arch.pb.h"
#include "modules/chassis/proto/chss_io.pb.h"

#include "modules/cambrian/brain/msgs_recv_tmpl.h"

namespace mstf {
namespace camb {

    using namespace apollo;
    using namespace /*mstf::camb::*/proto;
    using namespace /*mstf::camb::*/brain;

    class ReceiveMsg {
        public:
            ReceiveMsg(const std::shared_ptr<CambrianConfig> cc) {
                AINFO << "ReceiveMsg construct <MESSAGE IN>";
                subscribe_node_ = cyber::CreateNode(cc->
                        cmb_topic_conf().reader_node_name());
#ifdef CAMB_PKG_DBG
                AINFO << "reader node: \"" <<
                    cc->cmb_topic_conf().reader_node_name() << "\"";
#endif
                //data reader
                if (cc->cmb_topic_conf().has_input_slam_grid_name()) {
                    ReceiverGenerate<       //map
                        /*ventura::common_msgs::nav_msgs::*/OccupancyGrid>(
                                cc->cmb_topic_conf().
                                input_slam_grid_name().value());

                    ReceiverGenerate<       //pose
                        /*ventura::common_msgs::geometry_msgs::*/PoseStamped>(
                                cc->cmb_topic_conf().
                                input_slam_path_name().value());

                    ReceiverGenerate<ChassisMiscInfo>(
                                cc->cmb_topic_conf().
                                input_chs_misc_name().value());
#if 1
                    //testing
                    ReceiverGenerate<       //speed
                        /*ventura::common_msgs::geometry_msgs::*/Twist>(
                                cc->cmb_topic_conf().
                                output_speed_ctl_name().value());
#endif
                }
            }

            virtual ~ReceiveMsg() final {
#ifdef CAMB_PKG_DBG
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
#ifdef CAMB_PKG_DBG
        AINFO << "recive channel \"" << msg_name << "\"";
#endif
        AINFO << "R[" <<
            msg_reader_manager_pair_.size() <<
            "] \"" << cyber::message::GetMessageName<MessageT>() << "\"";

        msg_reader_manager_pair_[
            cyber::message::GetMessageName<MessageT>()] =
                std::make_shared<MsgsRcvTmpl<MessageT>>
                ([&]()->std::shared_ptr<cyber::Reader<MessageT>> {
                    //create writer
#ifdef CAMB_PKG_DBG
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
#ifdef CAMB_PKG_DBG
        AINFO << "build receiver \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_reader_manager_pair_) {
            if (x.first == cyber::message::GetMessageName<MessageT>())
                x.second->MsgsFlowoutControl(true);
        }
    }

    void ReceiveMsg::ReceiveChannelBuild() {
#ifdef CAMB_PKG_DBG
        AINFO << "build receiver all";
#endif
        for (auto x : msg_reader_manager_pair_) {
            x.second->MsgsFlowoutControl(true);
        }
    }

    template <typename MessageT> void
    ReceiveMsg::ReceiveChannelDestroy() {
#ifdef CAMB_PKG_DBG
        AINFO << "destroy receiver \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_reader_manager_pair_) {
            if (x.first == cyber::message::GetMessageName<MessageT>())
                x.second->MsgsFlowoutControl(false);
        }
    }

    void ReceiveMsg::ReceiveChannelDestroy() {
#ifdef CAMB_PKG_DBG
        AINFO << "destroy receiver all";
#endif
        for (auto x : msg_reader_manager_pair_) {
            x.second->MsgsFlowoutControl(false);
        }
    }

    template <typename MessageT> int
    ReceiveMsg::OnMessageReceive(const
            std::shared_ptr<MessageT>& msg) {
#ifdef CAMB_PKG_DBG
        //AINFO << "receive message \"" <<
            //cyber::message::GetMessageName<MessageT>() << "\"";
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

} //namespace camb
} //namespace mstf

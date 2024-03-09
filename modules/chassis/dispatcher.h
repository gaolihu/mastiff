#pragma once

#include "modules/chassis/devices/msgs_send_tmpl.h"

namespace mstf {
namespace chss {

    using namespace apollo;
    using namespace google::protobuf;

    using namespace /*mstf::chss::*/proto;
    using namespace /*mstf::chss::*/device;

    using namespace ventura::common_msgs::nav_msgs;
    using namespace ventura::common_msgs::sensor_msgs;

    class DispatchMsg {
        public:
            DispatchMsg(const std::shared_ptr<ChassisConfig> cc) {
                AINFO << "DispatchMsg construct <MESSAGE OUT>";
#ifdef CHSS_PKG_DBG
                AINFO << "writer node: \"" <<
                    cc->chs_topic_conf().
                    chassis_output_channel() << "\"";
#endif
                cc_ = cc;
                publish_node_ = cyber::CreateNode(cc_.lock()->
                        chs_topic_conf().chassis_output_channel());

                //data writers
                DispatcherGenerate<
                    /*ventura::common_msgs::nav_msgs::*/Odometry>(
                            cc_.lock()->chs_topic_conf().
                            output_odom_topic_name());
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/PointCloud>(
                            cc_.lock()->chs_topic_conf().
                            output_lpa_topic_name());
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/PointCloud2>(
                            cc_.lock()->chs_topic_conf().
                            output_lla_topic_name());
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/Imu>(
                            cc_.lock()->chs_topic_conf().
                            output_imu_topic_name());
                //rgb image
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/Image>(
                            cc_.lock()->chs_topic_conf().
                            output_irgb_topic_name());
                //depth image
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/Image>(
                            cc_.lock()->chs_topic_conf().
                            output_idep_topic_name());
                //image pcl
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/PointCloud2>(
                            cc_.lock()->chs_topic_conf().
                            output_ipa_topic_name());
                //mono left image
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/Image>(
                            cc_.lock()->chs_topic_conf().
                            output_imonol_topic_name());
                //mono left image
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/Image>(
                            cc_.lock()->chs_topic_conf().
                            output_imonor_topic_name());
#if 0
                //TODO
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/Image>(
                            cc_.lock()->chs_topic_conf().
                            output_cmd_topic_name());
                DispatcherGenerate<
                    /*ventura::common_msgs::sensor_msgs::*/Image>(
                            cc_.lock()->chs_topic_conf().
                            output_hcr_topic_name());
#endif
            }

            virtual ~DispatchMsg() final {
#ifdef CHSS_PKG_DBG
                AINFO << "DispatchMsg de-construct!";
#endif
                DispatchChannelDestroy();
            }

        private:
            template <typename MessageT>
            void DispatcherGenerate(const std::string&);

            template <typename MessageT>
            void DispatchChannelBuild(const std::string&);
            void DispatchChannelBuild();

            template <typename MessageT>
            void DispatchChannelDestroy(const std::string&);
            void DispatchChannelDestroy();

            template <typename MessageT>
            int OnMessageDispatch(const std::string&,
                    const std::shared_ptr<MessageT>&);

        private:
            std::unique_ptr<cyber::Node> publish_node_ = nullptr;

            //message & writer-creater pair
            std::map<const std::string,
                std::shared_ptr<MsgsImplItf>
                    > msg_writer_manager_pair_ {};

            std::weak_ptr<ChassisConfig> cc_ {};
    };

    template <typename MessageT> void
    DispatchMsg::DispatcherGenerate(const std::string& msg_topic) {
#ifdef CHSS_PKG_DBG
        AWARN << "build dispatch channel: \"" << msg_topic << "\"";
#endif
        AINFO << "DIS[" <<
            msg_writer_manager_pair_.size() <<
            "] [" << msg_topic <<
            "] \"" << cyber::message::GetMessageName<MessageT>() << "\"";

        msg_writer_manager_pair_[msg_topic +
            cyber::message::GetMessageName<MessageT>()] =
                std::make_shared<MsgsSendTmpl<MessageT>>
                ([&]()->std::shared_ptr<cyber::Writer<MessageT>> {
                    //create writer
#ifdef CHSS_PKG_DBG
                    AINFO << "creater writer " << "\"" <<
                        cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                    return publish_node_->CreateWriter<MessageT>(msg_topic);
                }, msg_topic);
        DispatchChannelBuild<MessageT>(msg_topic);
    }

    template <typename MessageT> void
    DispatchMsg::DispatchChannelBuild(const std::string& msg_topic) {
#ifdef CHSS_PKG_DBG
        AINFO << "build dispatcher \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"" <<
            " on " << msg_topic;
#endif
        for (auto x : msg_writer_manager_pair_) {
            if (x.first == msg_topic + cyber::message::GetMessageName<MessageT>())
                x.second->MsgsFlowoutControl(true);
        }
    }

    void DispatchMsg::DispatchChannelBuild() {
#ifdef CHSS_PKG_DBG
        AINFO << "build dispatcher all";
#endif
        for (auto x : msg_writer_manager_pair_) {
            x.second->MsgsFlowoutControl(true);
        }
    }

    template <typename MessageT> void
    DispatchMsg::DispatchChannelDestroy(const std::string& msg_topic) {
#ifdef CHSS_PKG_DBG
        AINFO << "destroy dispatcher \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_writer_manager_pair_) {
            if (x.first == msg_topic + cyber::message::GetMessageName<MessageT>())
                x.second->MsgsFlowoutControl(false);
        }
    }

    void DispatchMsg::DispatchChannelDestroy() {
#ifdef CHSS_PKG_DBG
        AINFO << "destroy dispatcher all";
#endif
        for (auto x : msg_writer_manager_pair_) {
            x.second->MsgsFlowoutControl(false);
        }
    }

    template <typename MessageT> int
    DispatchMsg::OnMessageDispatch(const std::string& msg_topic,
            const std::shared_ptr<MessageT>& msg) {
#ifdef CHSS_PKG_DBG
        AINFO << "dispatch message \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        for (auto x : msg_writer_manager_pair_) {
            if (x.first == msg_topic + cyber::message::GetMessageName<MessageT>()) {
                x.second->MessagePublish(msg);
            }
        }
        return 0;
    }

} //namespace chss
} //namespace mstf

#pragma once

#include "cyber/cyber.h"
#include "cyber/message/message_traits.h"

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/input_output_chs.pb.h"

#include "modules/chassis/devices/msg_transfer.h"

namespace mstf {
namespace chss {

    using namespace apollo;
    using namespace /*mstf::chss::*/proto;
    using namespace /*mstf::chss::*/device;

    using ImuPublisherGenerator =
        std::function<std::shared_ptr<cyber::Writer<IMUdata>>()>;
    using OdomPublisherGenerator =
        std::function<std::shared_ptr<cyber::Writer<OdomData>>()>;
    using LpaPublisherGenerator =
        std::function<std::shared_ptr<cyber::Writer<ventura::common_msgs::sensor_msgs::PointCloud>>()>;
    using CmdPublisherGenerator =
        std::function<std::shared_ptr<cyber::Writer<ChassisMixData>>()>;
    using CfdPublisherGenerator =
        std::function<std::shared_ptr<cyber::Writer<ChassisFacotryData>>()>;
    using ImgPublisherGenerator =
        std::function<std::shared_ptr<cyber::Writer<CameraCaptureFrame>>()>;
    using HcrPublisherGenerator =
        std::function<std::shared_ptr<cyber::Writer<HfChassisRaw>>()>;

    class DispatchChs {
        public:
            DispatchChs(const ChassissTopicConfig* ctp) {
                //TODO
#ifdef CHSS_PKG_DBG
                AINFO << "DispatchChs construct <message upstream>";
#endif
                chs_topic_conf_ = ctp;

                publish_node_ = cyber::CreateNode(chs_topic_conf_->
                        chassis_output_channel());
                RegisterLpaPublisher([&]()->std::shared_ptr<Writer<ventura::common_msgs::sensor_msgs::PointCloud>> {
                        AINFO << "create writer for ventura::common_msgs::sensor_msgs::PointCloud";
                        return publish_node_->CreateWriter<ventura::common_msgs::sensor_msgs::PointCloud>(
                                chs_topic_conf_->output_lpa_topic_name());
                        });
                LpaFlowSwitch(true);
                MsgTransfer::Instance()->SetTransferPublishers(
                        imu_publisher_, odom_publisher_,
                        lpa_publisher_, cmd_publisher_,
                        cfd_publisher_, img_publisher_,
                        hcr_publisher_);
            }
            virtual ~DispatchChs() final {
#ifdef CHSS_PKG_DBG
                AINFO << "DispatchChs de-construct!";
#endif
                ControlFlow<int>(false);
            }

            inline void RegisterImuPublisher(ImuPublisherGenerator imu) {
#ifdef CHSS_PKG_DBG
                AINFO << "register imu publisher generator";
#endif
                imu_publisher_generator_ = imu;
            }

            inline void RegisterOdomPublisher(OdomPublisherGenerator odom) {
#ifdef CHSS_PKG_DBG
                AINFO << "register odom publisher generator";
#endif
                odom_publisher_generator_ = odom;
            }

            inline void RegisterLpaPublisher(LpaPublisherGenerator lpa) {
#ifdef CHSS_PKG_DBG
                AINFO << "register lpa publisher generator";
#endif
                lpa_publisher_generator_ = lpa;
            }

            inline void RegisterCmdPublisher(CmdPublisherGenerator cmd) {
#ifdef CHSS_PKG_DBG
                AINFO << "register cmd publisher generator";
#endif
                cmd_publisher_generator_ = cmd;
            }

            inline void RegisterCfdPublisher(CfdPublisherGenerator cfd) {
#ifdef CHSS_PKG_DBG
                AINFO << "register cfd publisher generator";
#endif
                cfd_publisher_generator_ = cfd;
            }

            inline void RegisterImgPublisher(ImgPublisherGenerator img) {
#ifdef CHSS_PKG_DBG
                AINFO << "register img publisher generator";
#endif
                img_publisher_generator_ = img;
            }

            inline void RegisterHcrPublisher(HcrPublisherGenerator hcr) {
#ifdef CHSS_PKG_DBG
                AINFO << "register hcr publisher generator";
#endif
                hcr_publisher_generator_ = hcr;
            }

            void DispachFlowCtrl(const ChannelSwitch& cs) {
#ifdef CHSS_PKG_DBG
                AINFO << "chassis data flow ctrl:\n" << cs.DebugString();
#endif
                if (cs.has_imu_sw()) {
                    ImuFlowSwitch(cs.imu_sw().value());
                }

                if (cs.has_odom_sw()) {
                    OdomFlowSwitch(cs.odom_sw().value());
                }

                if (cs.has_lpa_sw()) {
                    LpaFlowSwitch(cs.lpa_sw().value());
                }

                if (cs.has_cmd_sw()) {
                    CmdFlowSwitch(cs.cmd_sw().value());
                }

                if (cs.has_cfd_sw()) {
                    CfdFlowSwitch(cs.cfd_sw().value());
                }

                if (cs.has_img_sw()) {
                    ImgFlowSwitch(cs.img_sw().value());
                }

                if (cs.has_hcr_sw()) {
                    HcrFlowSwitch(cs.hcr_sw().value());
                }
            }

            template <typename MessageT> bool ControlFlow(const bool);

        private:
            inline bool ImuFlowSwitch(const bool sw) {
                bool ret = false;
                if (sw)
                    ret = ConstructPublishChannel<IMUdata>();
                else
                    ret = UnconstructPublishChannel<IMUdata>();
#ifdef CHSS_PKG_DBG
                AINFO << "chassis imu publish channel " <<
                    (sw ? "construct" : "deconstruct") <<
                    (ret ? " OK" : " NG");
#endif
                return ret;
            }

            inline bool OdomFlowSwitch(const bool sw) {
                bool ret = false;
                if (sw)
                    ret = ConstructPublishChannel<OdomData>();
                else
                    ret = UnconstructPublishChannel<OdomData>();
#ifdef CHSS_PKG_DBG
                AINFO << "chassis odom publish channel " <<
                    (sw ? "construct" : "deconstruct") <<
                    (ret ? " OK" : " NG");
#endif
                return ret;
            }

            inline bool LpaFlowSwitch(const bool sw) {
                bool ret = false;
                if (sw)
                    ret = ConstructPublishChannel<ventura::common_msgs::sensor_msgs::PointCloud>();
                else
                    ret = UnconstructPublishChannel<ventura::common_msgs::sensor_msgs::PointCloud>();
#ifdef CHSS_PKG_DBG
                AINFO << "chassis lpa publish channel " <<
                    (sw ? "construct" : "deconstruct") <<
                    (ret ? " OK" : " NG");
#endif
                return ret;
            }

            inline bool CmdFlowSwitch(const bool sw) {
                bool ret = false;
                if (sw)
                    ret = ConstructPublishChannel<ChassisMixData>();
                else
                    ret = UnconstructPublishChannel<ChassisMixData>();
#ifdef CHSS_PKG_DBG
                AINFO << "chassis cmd publish channel " <<
                    (sw ? "construct" : "deconstruct") <<
                    (ret ? " OK" : " NG");
#endif
                return ret;
            }

            inline bool CfdFlowSwitch(const bool sw) {
                bool ret = false;
                if (sw)
                    ret = ConstructPublishChannel<ChassisFacotryData>();
                else
                    ret = UnconstructPublishChannel<ChassisFacotryData>();
#ifdef CHSS_PKG_DBG
                AINFO << "chassis cfd publish channel " <<
                    (sw ? "construct" : "deconstruct") <<
                    (ret ? " OK" : " NG");
#endif
                return ret;
            }

            inline bool ImgFlowSwitch(const bool sw) {
                bool ret = false;
                if (sw)
                    ret = ConstructPublishChannel<CameraCaptureFrame>();
                else
                    ret = UnconstructPublishChannel<CameraCaptureFrame>();
#ifdef CHSS_PKG_DBG
                AINFO << "chassis img publish channel " <<
                    (sw ? "construct" : "deconstruct") <<
                    (ret ? " OK" : " NG");
#endif
                return ret;
            }

            inline bool HcrFlowSwitch(const bool sw) {
                bool ret = false;
                if (sw)
                    ConstructPublishChannel<HfChassisRaw>();
                else
                    UnconstructPublishChannel<HfChassisRaw>();
#ifdef CHSS_PKG_DBG
                AINFO << "chassis hcr publish channel " <<
                    (sw ? "construct" : "deconstruct") <<
                    (ret ? " OK" : " NG");
#endif
                return ret;
            }

            template <typename MessageT> bool ConstructPublishChannel();
            template <typename MessageT> bool UnconstructPublishChannel();

        private:
            ImuPublisherGenerator imu_publisher_generator_ = nullptr;
            OdomPublisherGenerator odom_publisher_generator_ = nullptr;
            LpaPublisherGenerator lpa_publisher_generator_ = nullptr;
            CmdPublisherGenerator cmd_publisher_generator_ = nullptr;
            CfdPublisherGenerator cfd_publisher_generator_ = nullptr;
            ImgPublisherGenerator img_publisher_generator_ = nullptr;
            HcrPublisherGenerator hcr_publisher_generator_ = nullptr;

            ImuPublisher imu_publisher_ = nullptr;
            OdomPublisher odom_publisher_ = nullptr;
            LpaPublisher lpa_publisher_ = nullptr;
            CmdPublisher cmd_publisher_ = nullptr;
            CfdPublisher cfd_publisher_ = nullptr;
            ImgPublisher img_publisher_ = nullptr;
            HcrPublisher hcr_publisher_ = nullptr;

            int32_t hcr_cnt = 0;

            std::mutex hcr_mx;
            const ChassissTopicConfig* chs_topic_conf_  = nullptr;

            std::unique_ptr<Node> publish_node_ = nullptr;
    };

    template <typename MessageT>
    bool DispatchChs::ControlFlow(const bool sw) {
#ifdef CHSS_PKG_DBG
        AINFO << "control publish msg: " <<
            cyber::message::GetMessageName<MessageT>() <<
            (sw ? " construct" : " deconstruct");
#endif
        if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<IMUdata>()) {
            return ImuFlowSwitch(sw);
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<OdomData>()) {
            return OdomFlowSwitch(sw);
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ventura::common_msgs::sensor_msgs::PointCloud>()) {
            return LpaFlowSwitch(sw);
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ChassisMixData>()) {
            return CmdFlowSwitch(sw);
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ChassisFacotryData>()) {
            return CfdFlowSwitch(sw);
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<CameraCaptureFrame>()) {
            return ImgFlowSwitch(sw);
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<HfChassisRaw>()) {
            HcrFlowSwitch(sw);
        } else {
            ImuFlowSwitch(sw);
            OdomFlowSwitch(sw);
            LpaFlowSwitch(sw);
            CmdFlowSwitch(sw);
            CfdFlowSwitch(sw);
            ImgFlowSwitch(sw);
            return HcrFlowSwitch(sw);
        }

        return true;
    }

    template <typename MessageT>
    bool DispatchChs::ConstructPublishChannel() {
#ifdef CHSS_PKG_DBG
        AWARN << "build channel type: " <<
            cyber::message::GetMessageName<MessageT>();
#endif
        if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<IMUdata>()) {
            if (imu_publisher_) {
                AERROR << "imu publish channel aready build!";
                return false;
            }

            if (imu_publisher_generator_) {
                imu_publisher_ = imu_publisher_generator_();
                return true;
            } else {
                AERROR << "imu publish channel generator null!";
                return false;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<OdomData>()) {
            if (odom_publisher_) {
                AERROR << "odom publish channel aready build!";
                return false;
            }

            if (odom_publisher_generator_) {
                odom_publisher_ = odom_publisher_generator_();
                return true;
            } else {
                AERROR << "odom publish channel generator null!";
                return false;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ventura::common_msgs::sensor_msgs::PointCloud>()) {
            if (lpa_publisher_) {
                AERROR << "odmo publish channel aready build!";
                return false;
            }

            if (lpa_publisher_generator_) {
                lpa_publisher_ = lpa_publisher_generator_();
                return true;
            } else {
                AERROR << "lpa publish channel generator null!";
                return false;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ChassisMixData>()) {
            if (cmd_publisher_) {
                AERROR << "lpa publish channel aready build!";
                return false;
            }

            if (cmd_publisher_generator_) {
                cmd_publisher_ = cmd_publisher_generator_();
                return true;
            } else {
                AERROR << "cmd publish channel generator null!";
                return false;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ChassisFacotryData>()) {
            if (cfd_publisher_) {
                AERROR << "cmd publish channel aready build!";
                return false;
            }

            if (cfd_publisher_generator_) {
                cfd_publisher_ = cfd_publisher_generator_();
                return true;
            } else {
                AERROR << "cfd publish channel generator null!";
                return false;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<CameraCaptureFrame>()) {
            if (img_publisher_) {
                AERROR << "img publish channel aready build!";
                return false;
            }

            if (img_publisher_generator_) {
                img_publisher_ = img_publisher_generator_();
                return true;
            } else {
                AERROR << "img publish channel generator null!";
                return false;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<HfChassisRaw>()) {
            if (hcr_publisher_) {
                AERROR << "hcr publish channel aready build!";
                return false;
            }

            if (hcr_publisher_generator_) {
                hcr_publisher_ = hcr_publisher_generator_();
                return true;
            } else {
                AERROR << "hcr publish channel generator null!";
                return false;
            }
        }

        AERROR << "publish type: " <<
            cyber::message::GetMessageName<MessageT>() << " error!!";

        return false;
    }

    template <typename MessageT>
    bool DispatchChs::UnconstructPublishChannel() {
#ifdef CHSS_PKG_DBG
        AWARN << "unpublish type: " <<
            cyber::message::GetMessageName<MessageT>();
#endif
        if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<IMUdata>()) {
            if (imu_publisher_) {
                imu_publisher_.reset();
                return true;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<OdomData>()) {
            if (odom_publisher_) {
                odom_publisher_.reset();
                return true;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ventura::common_msgs::sensor_msgs::PointCloud>()) {
            if (lpa_publisher_) {
                lpa_publisher_.reset();
                return true;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ChassisMixData>()) {
            if (cmd_publisher_) {
                cmd_publisher_.reset();
                return true;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<ChassisFacotryData>()) {
            if (cfd_publisher_) {
                cfd_publisher_.reset();
                return true;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<CameraCaptureFrame>()) {
            if (img_publisher_) {
                img_publisher_.reset();
                return true;
            }
        } else if (cyber::message::GetMessageName<MessageT>() ==
                cyber::message::GetMessageName<HfChassisRaw>()) {
            if (hcr_publisher_) {
                hcr_publisher_.reset();
                return true;
            }
        }

        AERROR << "unpublish type: " <<
            cyber::message::GetMessageName<MessageT>() << " error!!";

        return false;
    }

} //namespace chss
} //namespace mstf

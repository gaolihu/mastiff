#pragma once

#include "cyber/common/log.h"
#include "cyber/common/macros.h"

#include "modules/chassis/proto/input_output_chs.pb.h"

#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/key_simulate.h"

#ifndef KEY_SIMULATE
#define KEY_SIMULATE 1
#endif

#ifndef RLS_DATA_PEEP
#define RLS_DATA_PEEP 1
#endif

namespace mstf {
namespace chss {
namespace device {

    using namespace apollo::cyber;
    using namespace /*mstf::chss::*/proto;
    using namespace /*mstf::chss::*/device;

    using ImuPublisher = std::shared_ptr<Writer<IMUdata>>;
    using OdomPublisher = std::shared_ptr<Writer<OdomData>>;
    using LpaPublisher = std::shared_ptr<Writer<LaserData>>;
    using CmdPublisher = std::shared_ptr<Writer<ChassisMixData>>;
    using CfdPublisher = std::shared_ptr<Writer<ChassisFacotryData>>;
    using ImgPublisher = std::shared_ptr<Writer<CameraCaptureFrame>>;
    using HcrPublisher = std::shared_ptr<Writer<HfChassisRaw>>;

    class MsgTransfer {
        public:
            DECLARE_SINGLETON(MsgTransfer)
            virtual ~MsgTransfer();

        public:
            void  Finish() {
                delete this;
            }

            //1, upstream interface in DispatchChs
            void SetTransferPublishers(const
                    ImuPublisher& imu,
                    const OdomPublisher& odom,
                    const LpaPublisher& laser,
                    const CmdPublisher& cmd,
                    const CfdPublisher& cfd,
                    const ImgPublisher& ccf,
                    const HcrPublisher& hcr) {
                imu_publisher_ = imu;
                odom_publisher_ = odom;
                lpa_publisher_ = laser;
                cmd_publisher_ = cmd;
                cfd_publisher_ = cfd;
                img_publisher_ = ccf;
                hcr_publisher_ = hcr;
            }

            //2, downstream interface in ReceiveCtrl
            int TransferChassisControl(const
                    std::shared_ptr<ChassisCtrl>& cc) {
                return DataTransact::Instance()->RecvChassCtrl(cc);
            }

            void SetChassisContrlSimItf(const SimulateProtoHandle& s) {
                sim_handle_ = s;
            }

        private:
#ifdef KEY_SIMULATE
            int SimTransferChassisControl(const
                    std::shared_ptr<Message>& m) {
                if (sim_handle_) {
                    return sim_handle_(m);
                } else {
                    return DataTransact::Instance()->RecvChassCtrl
                        (std::dynamic_pointer_cast<ChassisCtrl>(m));
                }
            }
#endif

            int DataDispatchImu(const std::shared_ptr<IMUdata>& imu) {
#ifdef RLS_DATA_PEEP
                AINFO << "release IMUdata:\n"
#if 0
                    << imu->DebugString();
#else
                ;
#endif
#endif
                if (imu_publisher_)
                    return imu_publisher_->Write(imu);

                AINFO << "release IMUdata error:\n" <<
                    imu->DebugString();

                return -1;
            }

            int DataDispatchOdom(const std::shared_ptr<OdomData>& odom) {
#ifdef RLS_DATA_PEEP
                AINFO << "release IMUdata:\n"
#if 0
                    << imu->DebugString();
#else
                ;
#endif
#endif
                if (odom_publisher_)
                    return odom_publisher_->Write(odom);

                AINFO << "release OdomData error:\n" <<
                    odom->DebugString();

                return -1;
            }

            int DataDispatchLaser(const std::shared_ptr<LaserData>& laser) {
#ifdef RLS_DATA_PEEP
                AINFO << "release IMUdata:\n"
#if 0
                    << imu->DebugString();
#else
                ;
#endif
#endif
                if (lpa_publisher_)
                    return lpa_publisher_->Write(laser);

                AINFO << "release LaserData error:\n" <<
                    laser->DebugString();

                return -1;
            }

            int DataDispatchMix(const std::shared_ptr<ChassisMixData>& mix) {
#ifdef RLS_DATA_PEEP
                AINFO << "release ChassisMixData:\n"
#if 0
                    << mix->DebugString();
#else
                ;
#endif
#endif
                if (cmd_publisher_)
                    return cmd_publisher_->Write(mix);

                AINFO << "release ChassisMixData error:\n" <<
                    mix->DebugString();

                return -1;
            }

            int DataDispatchFac(const std::shared_ptr<ChassisFacotryData>& fac) {
#ifdef RLS_DATA_PEEP
                AINFO << "release ChassisFacotryData:\n"
#if 0
                    << fac->DebugString();
#else
                ;
#endif
#endif
                if (cfd_publisher_)
                    return cfd_publisher_->Write(fac);

                AINFO << "release ChassisFacotryData error:\n" <<
                    fac->DebugString();

                return -1;
            }

            int DataDispatchImg(const std::shared_ptr<CameraCaptureFrame>& img) {
#ifdef RLS_DATA_PEEP
                AINFO << "release CameraCaptureFrame:\n"
#if 0
                    << img->DebugString();
#else
                ;
#endif
#endif
                if (img_publisher_)
                    return img_publisher_->Write(img);

                AINFO << "release CameraCaptureFrame error:\n" <<
                    img->DebugString();

                return -1;
            }

            int DataDispatchHcr(const std::shared_ptr<HfChassisRaw>& hcr) {
#ifdef RLS_DATA_PEEP
                AINFO << "release HfChassisRaw:\n"
#if 0
                    << hcr->DebugString();
#else
                ;
#endif
#endif
                if (hcr_publisher_)
                    return hcr_publisher_->Write(hcr);

                AINFO << "release HfChassisRaw error:\n" <<
                    hcr->DebugString();

                return -1;
            }

        private:
            ImuPublisher imu_publisher_ = nullptr;
            OdomPublisher odom_publisher_ = nullptr;
            LpaPublisher lpa_publisher_ = nullptr;
            CmdPublisher cmd_publisher_ = nullptr;
            CfdPublisher cfd_publisher_ = nullptr;
            ImgPublisher img_publisher_ = nullptr;
            HcrPublisher hcr_publisher_ = nullptr;

#ifdef KEY_SIMULATE
            std::shared_ptr<KeySimulate> key_sim_ = nullptr;
            SimulateProtoHandle sim_handle_ = nullptr;
#endif
    };

    MsgTransfer::MsgTransfer() {
        AINFO << "MsgTransfer singleton construct";
#ifdef KEY_SIMULATE
        key_sim_ = std::make_shared<KeySimulate>();
        key_sim_->RegisterHandle([&] (const
                    std::shared_ptr<Message>& m)->int {
                    return SimTransferChassisControl(m);
                });
        key_sim_->StartSimulate();
#endif
    }

    MsgTransfer::~MsgTransfer() {
        AINFO << "MsgTransfer singleton de-construct";
        DataTransact::Instance()->Finish();
    }

} //namespace device
} //namespace chss
} //namespace mstf

#pragma once

#include "modules/cambrian/proto/cambrian_data.pb.h"
#include "modules/cambrian/proto/cambrian_arch.pb.h"

#include "modules/cambrian/brain/chassis_itf.h"
#include "modules/cambrian/brain/bizlogic_itf.h"
#include "modules/cambrian/brain/infer_machine.h"

namespace mstf {
namespace camb {
namespace brain {

    class ImplementHub :
        public ChassisItf,
        public BizlogicItf,
        public InferMachine {
        public:
            ImplementHub(const BizLogicOpts*);
            virtual ~ImplementHub();

            void Init();
            void Start();
            void Stop();
            void Close();

            inline void RegisterMsgsDistributer(
                    const BaseMsgCaller& base,
                    const SlamDriveSvc& slam,
                    const NavDriveSvc& nav) {
#ifdef CAMB_PKG_DBG
                AINFO << "upward info flow register";
#endif
                //to upper channel: abstract robot
                //
                //= &BotItfImpl::lamada, find Message - Handles
                up_flow_msg_hdsl_ = base;
                //= &BotItfImpl::HandleDriveSlam
                up_flow_slam_svc_ = slam;
                //= &BotItfImpl::HandleDriveNavi
                up_flow_nav_svc_ = nav;
            }

            //1, overrides infer machine
            virtual void InferQueryInfos() override;
            virtual void InferPlanWorkingFlow() override;
            virtual void InferUpdateMultiZones() override;

            //2, overrides business client
            //TODO
            /*
                if (0) {
                    DriveRobotShiftWork(msg);
                }
                */
            virtual int DispachMessage(const
                    std::shared_ptr<Message>& msg) override {
                return up_flow_msg_hdsl_(msg);
            }

            virtual int DispachDrvSlam(const
                    std::shared_ptr<DriveSlam>& rqs,
                    std::shared_ptr<SlamDriveAck>& rsp) override {
                return up_flow_slam_svc_(rqs, rsp);
            }

            virtual int DispachDrvNavi(const
                    std::shared_ptr<DriveNavigation>& rqs,
                    std::shared_ptr<NavigationAck>& rsp) override {
                return up_flow_nav_svc_(rqs, rsp);
            }

        private:
            //helpers
            inline void SyncRemoteData() {
                up_flow_msg_hdsl_(std::make_shared<QueryVolatileInfo>());
            }
            inline void SyncLocalStat() {
                up_flow_msg_hdsl_(std::make_shared<QueryModeStatus>());
            }
            //TODO

        private:
            //handle director message
            BaseMsgCaller up_flow_msg_hdsl_;
            SlamDriveSvc up_flow_slam_svc_;
            NavDriveSvc up_flow_nav_svc_;
    };

} //namespace brain
} //namespace camb
} //namespace mstf

#pragma once

#include "modules/cambrian/proto/cambrian_arch.pb.h"
#include "modules/cambrian/proto/cambrian_data.pb.h"

#include "modules/cambrian/brain/chassis_itf.h"
#include "modules/cambrian/brain/bizlogic_itf.h"
#include "modules/cambrian/brain/infer_machine.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace /*mstf::camb::*/proto;

    using ImplementorRvs = std::function<int(const
            std::shared_ptr<Message>&)>;

    using SlamDrvRvs = std::function<int(const
            std::shared_ptr<DriveSlam>&,
            std::shared_ptr<SlamDriveAck>&)>;

    using NaviDrvRvs = std::function<int(const
            std::shared_ptr<DriveNavigation>&,
            std::shared_ptr<NavigationAck>&)>;

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
            void Loop(const int);

            void RegisterPublishHandle(const ImplementorRvs&,
                    const SlamDrvRvs&, const NaviDrvRvs&);

            void SetRandomEvents(const
                    std::shared_ptr<Message>&);

            void PushRandomEvents();

            inline void SetMultiZonesParam(const
                    std::shared_ptr<Message>& mzp) {
                //business_center_->SetMultiZonesParam(mzp);
            }

            inline void LoopIncrease() {
                loop_cnt_++;
            }

            inline uint32_t LoopCount() const {
                return loop_cnt_;
            }

            inline void NotifyMapUpdate() {
                //business_center_->NotifyMapUpdate();
            }

            //overrides infer machine
            virtual void RequestChassisInfo() override;
            virtual void PlanWorkingFlowPath() override;
            virtual void UpdateMultiZonesParam() override;


            virtual void GlobleMapUpdated() = 0;

            //publish random events
            virtual void PublishRandomEvents() = 0;

        private:
            //handle director message
            ImplementorRvs director_rvs_;
            SlamDrvRvs slam_rvs_;
            NaviDrvRvs navi_rvs_;

            inline int DispachMessage(const
                    std::shared_ptr<Message>& msg) {
                ACHECK(director_rvs_) << "dispach channel null!";
                if (0) {
                    DriveRobotShiftWork(msg);
                }
                return director_rvs_(msg);
            }

            inline int DispachDrvSlam(const
                    std::shared_ptr<DriveSlam>& rqs,
                    std::shared_ptr<SlamDriveAck>& rsp) {
                ACHECK(slam_rvs_) << "slam channel null!";
                return slam_rvs_(rqs, rsp);
            }

            inline int DispachDrvNavi(const
                    std::shared_ptr<DriveNavigation>& rqs,
                    std::shared_ptr<NavigationAck>& rsp) {
                ACHECK(navi_rvs_) << "navi channel null!";
                return navi_rvs_(rqs, rsp);
            }

            inline void SyncRemoteData() {
                DispachMessage(std::make_shared<QueryVolatileInfo>());
            }

        private:
            int upload_raw_interval_ms_ = 250; //ms
            std::chrono::steady_clock::time_point time_last_;

            uint32_t loop_cnt_ = 0;

            //peripheral values & status
            std::weak_ptr<RandomEvents> random_evts_;
    };

} //namespace brain
} //namespace camb
} //namespace mstf

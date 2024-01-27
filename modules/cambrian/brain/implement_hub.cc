#include "cyber/cyber.h"

#include "modules/cambrian/brain/implement_hub.h"

namespace mstf {
namespace camb {
namespace brain {

    ImplementHub::ImplementHub(const BizLogicOpts* iops) {
#ifdef CAMB_PKG_DBG
        AINFO << "ImplementHub construct ~ 0\n" <<
            iops->DebugString();
#endif
        /*
        business_center_ = std::make_unique
            <BusinessCenter>(p);
        business_center_->SetPsHandle(std::bind(
                    &ImplementHub::DispachMessage,
                    this, std::placeholders::_1),
                std::bind(&ImplementHub::DispachDrvSlam, this,
                    std::placeholders::_1, std::placeholders::_2),
                std::bind(&ImplementHub::DispachDrvNavi, this,
                    std::placeholders::_1, std::placeholders::_2));
                    */
    }

    ImplementHub::~ImplementHub() {
#ifdef CAMB_PKG_DBG
        AINFO << "ImplementHub de-construct";
#endif
        //TODO
    }

    void ImplementHub::Init() {
#ifdef CAMB_PKG_DBG
        AINFO << "hub init stat-machine & biz-logic";
#endif
        //business_center_->Prepare();
        InferMachine::Init();
    }

    void ImplementHub::Start() {
#ifdef CAMB_PKG_DBG
        AINFO << "ImplementHub start";
#endif
        //business_center_->Start();
        InferMachine::Start();
    }

    void ImplementHub::Stop() {
#ifdef CAMB_PKG_DBG
        AINFO << "ImplementHub stop";
#endif
        //business_center_->Terminate();
        InferMachine::Stop();
    }

    void ImplementHub::Close() {
#ifdef CAMB_PKG_DBG
        AINFO << "ImplementHub close";
#endif
        //business_center_->Terminate();
        //InferMachine::Close();
    }

    void ImplementHub::Loop(const int loop_cnt) {
#ifdef CAMB_PKG_DBG
        AINFO << "ImplementHub loop";
#endif
        //update volatile info
        if (LoopCount()  % loop_cnt == 0) { //200 * 300 ms = 60s
            SyncRemoteData();
            //NotifyMapUpdate();
        }

        /*
        if (business_center_.get() != nullptr) {
            business_center_->Recycle();
        }
        */
    }

    void ImplementHub::RegisterPublishHandle(const
            ImplementorRvs& dr,
            const SlamDrvRvs& slam_rvs,
            const NaviDrvRvs& navi_rvs) {
#ifdef CAMB_PKG_DBG
        AINFO << "upward info flow register";
#endif
        //to upper channel: abstract robot
        //= &BotItfImpl::lamada, find Message - Handles
        director_rvs_ = dr;
        //= &BotItfImpl::HandleDriveSlam
        navi_rvs_ = navi_rvs;
        //= &BotItfImpl::HandleDriveNavi
        slam_rvs_ = slam_rvs;
    }

    void ImplementHub::SetRandomEvents(const
            std::shared_ptr<Message>& r) {
        random_evts_ = std::dynamic_pointer_cast<RandomEvents>(r);
#ifdef CAMB_PKG_DBG
        AINFO << "bind shared random events" <<
            ", use_count: " << random_evts_.use_count() <<
            ", content p: " << random_evts_.lock() <<
            ", is expired: " << (random_evts_.expired() ? "YES" : "NO");
#endif
    }

    void ImplementHub::PushRandomEvents() {
        //business_center_->ImportRandomEvents(random_evts_.lock());
    }

    //infer machine override 1
    void ImplementHub::RequestChassisInfo() {
#ifdef CAMB_PKG_DBG
        AINFO << "check chassis infomation";
#endif
        //TODO
        //business_center_->Query();
    }

    //infer machine override 2
    void ImplementHub::PlanWorkingFlowPath() {
#ifdef CAMB_PKG_DBG
        AINFO << "plan working flow path";
#endif
        ACHECK(sm_mode_stat_.get() != nullptr) << "fault sm!";
        //ACHECK(business_center_.get() != nullptr) << "fault bc!";

        /*
        if (sm_mode_stat_->exec())
            business_center_->ExecRobotWork(sm_mode_stat_);
        //else
            //ignore exec
            */

        InferMachine::RestoreJobState();

        //update mode & status in remote side
        DispachMessage(std::make_shared<QueryModeStatus>());
    }

    //infer machine override 3
    void ImplementHub::UpdateMultiZonesParam() {
#ifdef CAMB_PKG_DBG
        AINFO << "update multi zones";
#endif
        DispachMessage(std::make_shared<QueryZones>());
    }

} //namespace brain
} //namespace camb
} //namespace mstf

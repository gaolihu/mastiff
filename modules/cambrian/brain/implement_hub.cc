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
    }

    ImplementHub::~ImplementHub() {
#ifdef CAMB_PKG_DBG
        AINFO << "ImplementHub de-construct";
#endif
        //TODO
    }

    void ImplementHub::Init() {
#ifdef CAMB_PKG_DBG
        AINFO << "impl-hub init biz-logic";
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

    void ImplementHub::InferQueryInfos() {
#ifdef CAMB_PKG_DBG
        AINFO << "check chassis infomation";
#endif
        //TODO
        //business_center_->Query();
    }

    void ImplementHub::InferPlanWorkingFlow() {
#ifdef CAMB_PKG_DBG
        AINFO << "plan working flow path";
#endif
        if (InferMachine::IsNeedExecJob())
            BizlogicItf::ExecRobotWork(
                    InferMachine::GetCurrentMode(),
                    InferMachine::GetCurrentStat(),
                    InferMachine::IsNewJob());
        //else
            //ignore exec

        InferMachine::RestoreJobState();

        //update mode & status in remote side
        SyncLocalStat();
    }

    void ImplementHub::InferUpdateMultiZones() {
#ifdef CAMB_PKG_DBG
        AINFO << "update multi zones";
#endif
        //DispachMessage(std::make_shared<QueryZones>());
    }

} //namespace brain
} //namespace camb
} //namespace mstf

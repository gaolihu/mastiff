#include "cyber/cyber.h"

#include "modules/cambrian/brain/bizlogic/task_sched.h"

namespace mstf {
namespace camb {
namespace brain {

    TaskSched::TaskSched(const std::string& xml) {
#ifdef CAMB_PKG_DBG
        AINFO << "TaskSched construct, bt-xml: " <<
            xml;
#endif
        xml_path_ = xml;

#if 1
        bt_factory_ = std::make_unique<BehaviorTreeFactory>();
#else
        bt_factory_.reset(new BehaviorTreeFactory(),
                [](BehaviorTreeFactory* b){
                AWARN << "delete bt ptr " << b;
                delete b;
                });
#endif
        share_info_ = std::make_shared<SwitchWorkFlow>();

        ChssElements::RegisterBtNodes(bt_factory_.get());
        //AssembleProc::RegisterBtNodes(bt_factory_.get());

        //bt_factory_->registerScriptingEnums<EE_DEVICES_MANAGE>();
        bt_factory_->registerBehaviorTreeFromFile(xml_path_);

        /*
        bt_factory_->registerNodeType<DumpMessage>("DumpMessage");
        bt_factory_->registerNodeType<ManagePeriphDevice>
            ("ManagePeriphDevice", this);
        bt_factory_->registerNodeType<ConfigMotorParam>
            ("ConfigMotorParam", this);
        bt_factory_->registerNodeType<RecordMotorStatus>
            ("RecordMotorStatus", this);
        bt_factory_->registerNodeType<SetPeriodeParams>
            ("SetPeriodeParams", this);
        bt_factory_->registerNodeType<SetRobotSpeed>
            ("SetRobotSpeed", this);
        bt_factory_->registerNodeType<SetElementAnchor>
            ("SetElementAnchor", this);
        bt_factory_->registerNodeType<UpdateRobotWorkingFlow>
            ("UpdateRobotWorkingFlow", this);
        bt_factory_->registerNodeType<HandleRobotWorkingFlow>
            ("HandleRobotWorkingFlow");
            */

#ifdef CAMB_PKG_DBG
        AINFO << "TaskSched construct OK!";
#endif
    }

    TaskSched::~TaskSched() {
#ifdef CAMB_PKG_DBG
        AINFO << "TaskSched de-construct: " <<
            bt_factory_.get();
#endif
    }

    //////////////////////Jobs//////////////////////
    void TaskSched::TerminateTicking() {
        AINFO << "terminate TaskSched ticking";

        //AssembleProc::NotifyTerminate();

        if (tree_.get() != nullptr) {
            tree_->haltTree();
            //
            //accelerate tree process
            //
            AINFO << "active fringe bt tree halt OK";
        } else {
            AWARN << "no active tree!";
        }
    }

    void TaskSched::ExecTaskAutoClean() {
#ifdef CAMB_PKG_DBG
        AINFO << "robot execute clean";
#endif
        std::lock_guard<std::mutex> lk(sch_mx_);

        //AssembleProc::NotifyStart();

        auto t = bt_factory_->createTree("MainTask-ExecuteAutoClean");

        tree_ = std::make_unique<Tree>(std::move(t));
        tree_->tickWhileRunning(std::chrono::milliseconds(20));
        tree_.reset();

        AINFO << "auto clean end!";

        /*
        if (anchor)
            anchor();
            */

        AWARN << "clean finished!";
    }

    //testing
    void TaskSched::ExecuteRrtExplore(const
            std::function<void()>& anchor) {
#ifdef CAMB_PKG_DBG
        AINFO << "robot do rrt explore";
#endif
        std::lock_guard<std::mutex> lk(sch_mx_);

        auto t = bt_factory_->createTree("SubTask-RrtExplore");

        tree_ = std::make_unique<Tree>(std::move(t));
        tree_->tickWhileRunning(std::chrono::milliseconds(20));
        tree_.reset();

        AINFO << "rrt explore end!";

        if (anchor)
            anchor();

        AWARN << "rrt finished!";
    }

    void TaskSched::ExecutePlanPath(const
            std::function<void()>& anchor) {
#ifdef CAMB_PKG_DBG
        AINFO << "robot execute path plan";
#endif
        std::lock_guard<std::mutex> lk(sch_mx_);

        auto t = bt_factory_->createTree("SubTask-PlanPath");

        tree_ = std::make_unique<Tree>(std::move(t));
        tree_->tickWhileRunning(std::chrono::milliseconds(20));
        tree_.reset();

        AINFO << "plan path end!";

        if (anchor)
            anchor();

        AWARN << "plan path finished!";
    }

    void TaskSched::ExecuteTraceTravel(const
            std::function<void()>& anchor) {
#ifdef CAMB_PKG_DBG
        AINFO << "robot execute trace travel";
#endif
        std::lock_guard<std::mutex> lk(sch_mx_);

        auto t = bt_factory_->createTree("SubTask-TraceTravel");

        tree_ = std::make_unique<Tree>(std::move(t));
        tree_->tickWhileRunning(std::chrono::milliseconds(20));
        tree_.reset();

        AINFO << "tracking end!";

        if (anchor)
            anchor();

        AWARN << "tracing finished!";
    }

    void TaskSched::ExecuteAlongWall(const
            std::function<void()>& anchor) {
#ifdef CAMB_PKG_DBG
        AINFO << "robot execute edgewise";
#endif
        std::lock_guard<std::mutex> lk(sch_mx_);

        auto t = bt_factory_->createTree("SubTask-Edgewise");

        tree_ = std::make_unique<Tree>(std::move(t));
        tree_->tickWhileRunning(std::chrono::milliseconds(20));
        tree_.reset();

        AINFO << "edgewise end!";

        if (anchor)
            anchor();

        AINFO << "edgewise finished!";
    }

    void TaskSched::ExecutePause() {
#ifdef CAMB_PKG_DBG
        AINFO << "robot execute pause ";
#endif
        //AssembleProc::NotifyPause();

        share_info_->set_ctl_work_flow(true);
        share_info_->set_ctrl(E_CTRL_TYPE_PAUSE);
    }

    void TaskSched::ExecuteResume() {
#ifdef CAMB_PKG_DBG
        AINFO << "robot execute resume";
#endif
        //AssembleProc::NotifyResume();

        share_info_->set_ctl_work_flow(true);
        share_info_->set_ctrl(E_CTRL_TYPE_RESUME);
    }

    void TaskSched::ExecuteFinish() {
#ifdef CAMB_PKG_DBG
        AINFO << "robot execute finish";
#endif
        //AssembleProc::NotifyTerminate();

        share_info_->set_ctl_work_flow(true);
        share_info_->set_ctrl(E_CTRL_TYPE_FINISH);
    }

    //////////////////////Jobs//////////////////////

} //namespace brain
} //namespace camb
} //namespace mstf

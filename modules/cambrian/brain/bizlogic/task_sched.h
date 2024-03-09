#pragma once

//#include "modules/cambrian/brain/bizlogic/action_set.h"
#include "modules/cambrian/brain/bizlogic/chss_elements.h"

using namespace BT;
using namespace mstf;
using namespace camb;
using namespace brain;

namespace mstf {
namespace camb {
namespace brain {

    using namespace google::protobuf;

    class TaskSched :
        public ChssElements {
        public:
            TaskSched(const std::string&);
            virtual ~TaskSched();

            //////////////////////Jobs//////////////////////
            void TerminateTicking();

            void ExecTaskAutoClean();

            //sub tasks, for testing modules
            void ExecuteRrtExplore(const std::function<void()>&);
            void ExecutePlanPath(const std::function<void()>&);
            void ExecuteTraceTravel(const std::function<void()>&);
            void ExecuteAlongWall(const std::function<void()>&);

            void ExecutePause();
            void ExecuteResume();
            void ExecuteFinish();

            //override element ctrl
            inline virtual NodeStatus ElmClearCtrl() override {
                //elm_chs_ctrl->Clear();
                return NodeStatus::SUCCESS;
            }

            inline virtual NodeStatus ElmReleaseCtrl() override {
                return NodeStatus::SUCCESS;
            }

            inline std::shared_ptr<SwitchWorkFlow> GetWorkFlow() {
                return share_info_;
            }

        private:
            std::string xml_path_ = "";

            std::unique_ptr<Tree> tree_;
            std::unique_ptr<BehaviorTreeFactory> bt_factory_;

            //working flow control
            std::shared_ptr<SwitchWorkFlow> share_info_ = nullptr;

            //sync
            std::mutex sch_mx_;

    };

}; //brain
}; //camb
}; //mstf

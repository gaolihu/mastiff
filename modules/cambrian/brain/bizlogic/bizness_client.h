#pragma once

#include <thread>
#include <boost/thread/condition_variable.hpp>

#include "cyber/common/log.h"

#define KEY_SIMULATE
#ifdef KEY_SIMULATE
//For testing
#include "modules/cambrian/brain/bizlogic/key_simulate.h"
#endif

#include "modules/cambrian/brain/bizlogic/task_sched.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace /*mstf::camb::*/proto;
    using namespace google::protobuf;


    //using TaskVoidCaller = std::function<void()>;

    using BaseMsgCaller = std::function<int(const
            std::shared_ptr<Message>&)>;

    using SlamDriveSvc = std::function<int(const
            std::shared_ptr<DriveSlam>&,
            std::shared_ptr<SlamDriveAck>&)>;

    using NavDriveSvc = std::function<int(const
            std::shared_ptr<DriveNavigation>&,
            std::shared_ptr<NavigationAck>&)>;

    class BiznessClient {
        public:
            BiznessClient();
            virtual ~BiznessClient();

            inline void BuildMsgChannel(
                    const BaseMsgCaller& base,
                    const SlamDriveSvc& slam,
                    const NavDriveSvc& nav) {
                //= &BizlogicItf::DispachMessage
                up_flow_msg_hdsl_ = base;
                //= &BizlogicItf::DispachDrvSlam
                up_flow_slam_svc_ = slam;
                //= &BizlogicItf::DispachDrvNavi
                up_flow_nav_svc_ = nav;
            }

            void ExecRobotWork(
                    const EE_ROBOT_MODE,
                    const EE_ROBOT_STATUS,
                    const bool);

        private:
            void StartNewWork(const EE_ROBOT_MODE);
            void PauseCurrentWork();
            void ResumeCurrentWork();
            void TerminateCurrentWork();

            void Run();
            void Recycle();
            void RecycleMission();

            void StartTangibleTask(const
                    std::function<void()>&);

        private:
            std::shared_ptr<TaskSched> tsk_schedulor_ {};

            //channels up flow
            BaseMsgCaller up_flow_msg_hdsl_;
            SlamDriveSvc up_flow_slam_svc_;
            NavDriveSvc up_flow_nav_svc_;

#ifdef KEY_SIMULATE
            std::unique_ptr<KeySimulate> key_sim_ {};
#endif

            std::atomic<bool> at_work_ = false;
            std::atomic<bool> at_recycle_ = false;

            std::thread working_thread_;
            std::thread recycle_thread_;

            boost::mutex sv_mx_;

            boost::mutex bl_mx_;
            boost::mutex rc_mx_;
            boost::condition_variable bs_cv_;
    };

} //namespace brain
} //namespace camb
} //namespace mstf

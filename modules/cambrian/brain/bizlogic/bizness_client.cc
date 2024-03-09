#include "modules/cambrian/brain/bizlogic/bizness_client.h"

namespace mstf {
namespace camb {
namespace brain {

    BiznessClient::BiznessClient() {
#ifdef CAMB_PKG_DBG
        AINFO << "BiznessClient construct ~ 1";
#endif
#ifdef KEY_SIMULATE
        key_sim_ = std::make_unique<KeySimulate>();
        key_sim_->RegisterHandle([&] (const
                    std::shared_ptr<Message>& m)->int {
                return up_flow_msg_hdsl_(m);
                });
        key_sim_->StartSimulate();
#endif
        tsk_schedulor_ = std::make_shared<TaskSched>("xml");
    }

    BiznessClient::~BiznessClient() {
#ifdef CAMB_PKG_DBG
        AINFO << "BiznessClient de-construct";
#endif
    }

    void BiznessClient::ExecRobotWork(
            const EE_ROBOT_MODE mode,
            const EE_ROBOT_STATUS stat,
            const bool is_new) {
#ifdef CAMB_PKG_DBG
        AINFO << "exec robot working bussiness";
#endif
        switch (stat) {
            case E_STATUS_NOWORK: {
#ifdef CAMB_PKG_DBG
                AWARN << "nowork!";
#endif
            }
            break;

            case E_STATUS_PAUSING: {
                PauseCurrentWork();
            }
            break;

            case E_STATUS_WORKING: {
                if (is_new) {
                    //start a new job
                    StartNewWork(mode);
                } else {
                    //resume last job
                    ResumeCurrentWork();
                }
            }
            break;

            case E_STATUS_STOPWORK: {
                TerminateCurrentWork();
            }
            break;

            default:
            AERROR << "infer machine parse error!";
        }
#ifdef CAMB_PKG_DBG
        //AINFO << "exec robot working OK";
#endif
    }

    void BiznessClient::StartNewWork(const
            EE_ROBOT_MODE mode) {
#ifdef CAMB_PKG_DBG
        AINFO << "new work start:" << mode;
#endif
        Run();

        switch (mode) {
            case E_MODE_AUTO_CLEAN:
                StartTangibleTask([&](){
                        AINFO << "exec auto clean!";
                        tsk_schedulor_->
                        TaskSched::ExecTaskAutoClean();
                        });
                break;
            case E_MODE_AREA_CLEAN:
                break;
            case E_MODE_ROOM_CLEAN:
                break;
            case E_MODE_POINT_CLEAN:
                break;
            case E_MODE_REMOTE_CLEAN:
                break;
            case E_MODE_LOCAL_CLEAN:
                break;
            case E_MODE_TRACK_CLEAN:
                break;
            case E_MODE_STAND_BY:
                break;
            case E_MODE_DOCKING:
                break;
            case E_MODE_CHARGING:
                break;
            case E_MODE_CHARGE_END:
                break;
            case E_MODE_IN_FAULT:
                break;
            case E_MODE_DORMANT:
                break;
            case E_MODE_ALONG_WALL:
                break;
            case E_MODE_SWIFT_MAP:
                break;
            case E_MODE_RELOCATION:
                break;
            default:
                break;
        }
    }

    void BiznessClient::PauseCurrentWork() {
        //TaskSched::ExecutePause();
    }

    void BiznessClient::ResumeCurrentWork() {
        //TaskSched::ExecuteResume();
    }

    void BiznessClient::TerminateCurrentWork() {
#ifdef CAMB_PKG_DBG
        AINFO << "start to terminate current work";
#endif
        if (!at_work_) {
            AWARN << "not at work, ignore terminate!";
            return;
        }

        //TaskSched::ExecuteFinish();
        //TaskSched::TerminateTicking();

        RecycleMission();

        //TaskSched::ChassisTerminate();

#ifdef CAMB_PKG_DBG
        AINFO << "terminate current work OK";
#endif
    }

    void BiznessClient::Run() {
#ifdef CAMB_PKG_DBG
        AINFO << "business run!";
#endif
        //TaskSched::ChassisRun();
    }

    void BiznessClient::StartTangibleTask(const
            std::function<void()>& task) {
#ifdef CAMB_PKG_DBG
        //AINFO << "start tangible task, mzp:\n" <<
            //mzp_.lock()->DebugString();
#endif
        at_work_ = true;

        working_thread_ = std::thread(std::ref(task)/*,
                std::ref(recycler)*/);
        recycle_thread_ = std::thread([&]() {
                boost::unique_lock<boost::mutex> lk(bl_mx_);
                while (at_work_) {
                if (bs_cv_.wait_for(lk, boost::chrono::
                            milliseconds(4000)) ==
                        boost::cv_status::timeout) {
                //continue
                //AINFO << "recycle timeout";
                } else {
#ifdef CAMB_PKG_DBG
                AINFO << "working thread exit...";
#endif
                break;
                }
                }
                AINFO << "working thread start join!";
                working_thread_.join();
                AINFO << "working thread recycled ok!";

                AINFO << "recycle thread exit!";
                });
#ifdef CAMB_PKG_DBG
        AINFO << "tangible working task detached!";
#endif
    }

    void BiznessClient::RecycleMission() {
        AINFO << "recycle active mission1 start!";
        bs_cv_.notify_all();
        /*
        boost::this_thread::sleep_for(boost::
                chrono:: milliseconds(50));
                */
        at_work_ = false;
        at_recycle_ = true;

        //notity upper layer that the mission is end
        {
            auto r = std::make_shared<RobotModeStatus>();
            r->mutable_task_shift()->set_task_finished(true);
            r->mutable_task_shift()->set_ctrl(E_CTRL_TYPE_FINISH);
            up_flow_msg_hdsl_(r);
        }

        AINFO << "recycle with mode & status feedback!";
    }

    void BiznessClient::Recycle() {
#ifdef CAMB_PKG_DBG
        //AINFO << "recycle!";
#endif
#if 0
        boost::unique_lock<boost::mutex> lk(rc_mx_);
        if (bs_cv_.wait_for(lk, boost::chrono::
                    milliseconds(30)) ==
                boost::cv_status::timeout) {
            //do nothing
            //AINFO << "--------main recycle timeout--------";
        }
#else
        //POSIX code
        pthread_cond_t cond;
        pthread_mutex_t mutex;

        long long nsec;
        struct timeval now;
        struct timespec abstime;

        int msec = 300;

        gettimeofday(&now, NULL);
        nsec = now.tv_usec * 1000 + (msec % 1000) * 1000000;
        abstime.tv_nsec = nsec % 1000000000;
        abstime.tv_sec = now.tv_sec + nsec / 1000000000 + msec / 1000;

        AINFO << "pthread_cond_timedwait: " <<
            pthread_cond_timedwait(&cond, &mutex, &abstime);

#endif

        if (at_recycle_ && recycle_thread_.joinable()) {
            recycle_thread_.join();
#ifdef CAMB_PKG_DBG
            AINFO << "main recycle OK!";
#endif
            at_recycle_ = false;
        }
    }

} //namespace brain
} //namespace camb
} //namespace mstf

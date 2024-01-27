#pragma once

#include "cyber/common/log.h"

#include "modules/cambrian/proto/cambrian_data.pb.h"

#include "modules/cambrian/brain/message_pool.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace google::protobuf;
    using namespace /*mstf::camb::*/proto;

    class InferMachine {
        public:
            InferMachine();
            virtual ~InferMachine();

        public:
            void Init();
            void Start();
            void Stop();

            //exteral event drive working
            void PushPowerKey(const EE_CMD_FROM_WHO);
            void PushPowerKeyLong(const EE_CMD_FROM_WHO);
            void PushHomeKey(const EE_CMD_FROM_WHO);
            void PushHomeKeyLong(const EE_CMD_FROM_WHO);

            //exteral event change status
            void EventOnDock(const bool);
            void EventChargeFull(const bool);

            std::shared_ptr<Message> ExposeStatMode();

            void RestoreJobState() {
#ifdef CAMB_PKG_DBG
                //AINFO << "restore infer machine status";
#endif
                exec_ = false;
                job_new_ = false;
                if (sm_mode_stat_.get() != nullptr) {
                    sm_mode_stat_->set_exec(false);
                    sm_mode_stat_->set_mission_new(false);
                }
            }

        public:
            //virtual APIs
            virtual void RequestChassisInfo() = 0;
            virtual void PlanWorkingFlowPath() = 0;
            virtual void UpdateMultiZonesParam() = 0;

        private:
            std::string GetDriveString(
                    const EE_CMD_FROM_WHO = E_CMD_FROM_KEY,
                    const EE_ACT_KEY_TYPE = E_KEY_POWER,
                    const EE_CONTROL_TYPE = E_CTRL_TYPE_START) const;
            std::string GetStatString(const int = -1) const;
            std::string GetStatStrLast(const int = -1) const;
            std::string GetModeString(const int = -1) const;
            std::string GetModeStrLast(const int = -1) const;

            bool IsPending() const {
                return (status_pending_ | mode_pending_) ?
                    true : false;
            }

            void FixModeStatus();
            void ShowModeStatus(const
                    EE_CMD_FROM_WHO = E_CMD_FROM_SENTINEL,
                    const EE_ACT_KEY_TYPE = E_KEY_SENTINEL,
                    const EE_CONTROL_TYPE = E_CTRL_TYPE_SENTINEL) const;
            std::string ShowRobotDriving(const EE_CONTROL_TYPE,
                    const EE_ACT_KEY_TYPE,
                    const EE_CLEAN_MODE,
                    const EE_CMD_FROM_WHO) const;
            std::string ShowRobotJobExec() const;

            inline EE_ROBOT_MODE RobotModeDeduction(
                    const EE_ACT_KEY_TYPE act,
                    const EE_CLEAN_MODE mode) const {
                //mode deduction
                if (act == E_KEY_POWER) {
                    if (mode == E_CLEAN_GLOBAL)
                        return E_MODE_AUTO_CLEAN;
                    else if (mode == E_CLEAN_AREA)
                        return E_MODE_AREA_CLEAN;
                    else if (mode == E_CLEAN_ROOM)
                        return E_MODE_ROOM_CLEAN;
                    else if (mode == E_CLEAN_POINT)
                        return E_MODE_POINT_CLEAN;
                    else if (mode == E_CLEAN_REMOTE)
                        return E_MODE_REMOTE_CLEAN;
                    else if (mode == E_CLEAN_LOCAL)
                        return E_MODE_LOCAL_CLEAN;
                    else if (mode == E_CLEAN_TRACK)
                        return E_MODE_TRACK_CLEAN;
                    else if (mode == E_CLEAN_EDGEWISE)
                        return E_MODE_ALONG_WALL;
                    else if (mode == E_CLEAN_SENTINEL)
                        return mode_;
                } else if (act == E_KEY_DOCK) {
                        return E_MODE_DOCKING;
                }

                AERROR << "mode: " << mode <<
                    ", deduction wrong!!!";

                return E_MODE_NULL;
            }

            bool DriveRobotWorking(
                    const EE_CONTROL_TYPE = E_CTRL_TYPE_SENTINEL,
                    const EE_ACT_KEY_TYPE = E_KEY_POWER,    //default power end task
                    const EE_CLEAN_MODE = E_CLEAN_GLOBAL,   //default auto clean
                    const EE_CMD_FROM_WHO = E_CMD_FROM_KEY); //default key

            void UpdataInferMachine();
            //TODO

        public:
            //for driving robot to work
            bool DriveRobotWorking(const MissionCommand&);
            //for pausing/continue/stop..
            bool DriveRobotShiftWork(const
                    std::shared_ptr<Message>& msg);

            void UpdataBattery(const uint32_t);
            void UpdataFan(const uint32_t);
            void UpdataFlow(const uint32_t );
            void UpdataBoost(const uint32_t);
            inline std::shared_ptr<MessagePool> GetMessagePool() {
                return cache_pool_;
            }

        private:
            std::shared_ptr<MessagePool> cache_pool_ {};
            /*
            std::tuple<bool, bool> pending_;
            std::tuple<EE_ROBOT_STATUS, EE_ROBOT_MODE> mode_stat_last_;
            */

            //current robot status
            EE_ROBOT_STATUS status_ = E_STATUS_NULL;
            bool status_pending_ = true;
            //last robot status
            EE_ROBOT_STATUS last_status_ = E_STATUS_NULL;

            //current robot mode
            EE_ROBOT_MODE mode_ = E_MODE_NULL;
            bool mode_pending_ = true;
            //last robot mode
            EE_ROBOT_MODE last_mode_ = E_MODE_NULL;

            //current clean mode
            EE_CLEAN_MODE work_mode_;

            SessionInfo session_;

            EE_ERROR_TYPES error_ = E_ERR_NONE;
            int16_t battery_ = -1;
            int16_t fan_ = -1;
            int16_t flow_ = -1;
            bool boost_ = false;
            bool exec_ = false;
            bool job_new_ = false;
            bool full_ = false;

        public:
            std::shared_ptr<RobotModeStatus> sm_mode_stat_ = nullptr;
    };

} //namespace brain
} //namespace camb
} //namespace mstf

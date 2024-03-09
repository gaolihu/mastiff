#pragma once

#include "modules/cambrian/brain/bizlogic/chss_elements.h"

namespace mstf {
namespace camb {
namespace brain {

    class DumpMessage : public SyncActionNode {
        public:
            DumpMessage(const std::string& name,
                    const NodeConfig& config)
                : SyncActionNode(name, config)
            { }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts for message dump";
#endif
                return { InputPort<std::string>("message") };
            }

            NodeStatus tick() override {
                Expected<std::string> msg =
                    getInput<std::string>("message");
                if (!msg) {
                    throw BT::RuntimeError(
                            "missing required input [message]: ",
                            msg.error() );
                }
                AWARN << " [ TaskSched BT DumpMsg ] >> " <<
                    msg.value();
                return NodeStatus::SUCCESS;
            }
    };

    class ManagePeriphDevice : public SyncActionNode {
        public:
            ManagePeriphDevice(const std::string& name,
                    const NodeConfig& config,
                    TaskSched* TaskSched) :
                SyncActionNode(name, config) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "[construct] manage periph device: " << name;
#endif
                    sch_ = TaskSched;
                }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts for manage periph dev";
#endif
                return { InputPort<std::string>("manage_status") };
            }

            NodeStatus tick() override {
#ifdef CAMB_PKG_DBG
                //AINFO << "tick for manage periph device";
#endif

                /*
                Expected<ChassisPeripheralManage> mana_status =
                    getInput<ChassisPeripheralManage>("manage_status");

                if (!mana_status) {
                    AERROR << "manage peripheral get error!";
                    return NodeStatus::FAILURE;
                }


                if (sch_) {
                    ChassisPeripheralManage cpm = mana_status.value();
                    sch_->ChssElements::ElecChsManageDevs(cpm);
                }
                */

                return NodeStatus::SUCCESS;
            }

        private:
            TaskSched* sch_ = nullptr;
    };

    class ConfigMotorParam : public SyncActionNode {
        public:
            ConfigMotorParam(const std::string& name,
                    const NodeConfig& config,
                    TaskSched* TaskSched) :
                SyncActionNode(name, config) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "[construct] motor parameters: " << name;
#endif
                    sch_ = TaskSched;
                }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts for config periph params";
#endif
                return { InputPort<std::string>("periph_param") };
            }

            NodeStatus tick() override {
#ifdef CAMB_PKG_DBG
                //AINFO << "tick for config motor periph_param";
#endif

                /*
                Expected<ChassisPeripheralParams> mana_param =
                    getInput<ChassisPeripheralParams>("periph_param");

                if (!mana_param) {
                    AERROR << "config motor param get error!";
                    return NodeStatus::FAILURE;
                }


                if (sch_) {
                    ChassisPeripheralParams cpp = mana_param.value();
                    sch_->ChssElements::ElecChsSetPeriphParam(cpp);
                }
                */

                return NodeStatus::SUCCESS;
            }

        private:
            TaskSched* sch_ = nullptr;
    };

    class RecordMotorStatus : public SyncActionNode {
        public:
            RecordMotorStatus(const std::string& name,
                    const NodeConfig& config,
                    TaskSched* TaskSched) :
                SyncActionNode(name, config) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "[construct] record motor status: " << name;
#endif
                    sch_ = TaskSched;
                }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts for config periph params";
#endif
                return { OutputPort<std::string>("periph_param") };
            }

            NodeStatus tick() override {
#ifdef CAMB_PKG_DBG
                //AINFO << "tick for record motor periph_param";
#endif
                /*
                setOutput("periph_param",
                        sch_->ChssElements::ElecChsGetPeriphParam());
                        */

                return NodeStatus::SUCCESS;
            }

        private:
            TaskSched* sch_ = nullptr;
    };

    class SetPeriodeParams : public SyncActionNode {
        public:
            SetPeriodeParams(const std::string& name,
                    const NodeConfig& config,
                    TaskSched* TaskSched) :
                SyncActionNode(name, config) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "[construct] set MCU sensor periods: " << name;
#endif
                    sch_ = TaskSched;
                }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts setting period param";
#endif
                return { /*InputPort<ChassisPeriodParams>("period_params")*/ };
            }

            NodeStatus tick() override {
#ifdef CAMB_PKG_DBG
                //AINFO << "tick for setting chassis periods";
#endif
                /*
                Expected<ChassisPeriodParams> cpp =
                    getInput<ChassisPeriodParams>("period_params");

                if (!cpp) {
                    AERROR << "period params get error!";
                    return NodeStatus::FAILURE;
                }

                //setOutput("param_period_prepare", cpp);

                if (sch_) {
                    sch_->ChssElements::ElecChsPeriodParam(cpp.value());
                }
                */

                return NodeStatus::SUCCESS;
            }

        private:
            TaskSched* sch_ = nullptr;
    };

    class SetRobotSpeed : public SyncActionNode {
        public:
            SetRobotSpeed(const std::string& name,
                    const NodeConfig& config,
                    TaskSched* TaskSched) :
                SyncActionNode(name, config) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "[construct] set robot speed: " << name;
#endif
                    sch_ = TaskSched;
                }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts setting robot speed";
#endif
                return { /*InputPort<ChassisSpeedParams>("robot_speed")*/ };
            }

            NodeStatus tick() override {
                /*
                Expected<ChassisSpeedParams> csp =
                    getInput<ChassisSpeedParams>("robot_speed");

                if (!csp) {
                    AERROR << "robot speed get error!";
                    return NodeStatus::FAILURE;
                }

                if (sch_) {
                    sch_->ChssElements::ElecChsSpeedParam(csp.value());
                }
                */
//#ifdef CAMB_PKG_DBG
#if 0
                AINFO << "tick for setting speed" <<
                    ", line: " << csp->line <<
                    ", angel: " << csp->line;
#endif
                return NodeStatus::SUCCESS;
            }

        private:
            TaskSched* sch_ = nullptr;
    };

    class SetElementAnchor : public SyncActionNode {
        public:
            SetElementAnchor(const std::string& name,
                    const NodeConfig& config,
                    TaskSched* TaskSched) :
                SyncActionNode(name, config) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "[construct] set chasiss anchor: " << name;
#endif
                    sch_ = TaskSched;
                }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts setting anchor";
#endif
                return { };
            }

            NodeStatus tick() override {
#ifdef CAMB_PKG_DBG
                //AINFO << "tick setting anchor";
#endif
                if (sch_) {
                    //return sch_->ChssElements::ElecChsAnchor();
                }

                return NodeStatus::FAILURE;
            }

        private:
            TaskSched* sch_ = nullptr;
    };

    class UpdateRobotWorkingFlow : public StatefulActionNode {
        public:
            UpdateRobotWorkingFlow(const std::string& name,
                    const NodeConfig& config,
                    TaskSched* TaskSched) :
                StatefulActionNode(name, config) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "[construct] update robot working flow: " << name;
#endif
                    sch_ = TaskSched;
                }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts update robot working flow";
#endif
                return { OutputPort<SwitchWorkFlow>("working_flow") };
            }

            virtual NodeStatus onStart() override {
                std::string wf = ((sch_->GetWorkFlow()->
                        ctl_work_flow()) ? "1;" : "0;");

                switch (sch_->GetWorkFlow()->ctrl()) {
                case E_CTRL_TYPE_SENTINEL:
                case E_CTRL_TYPE_PROCESS:
                case E_CTRL_TYPE_RESTART:
                case E_CTRL_TYPE_QUERY:
                case E_CTRL_TYPE_NONE:
                    wf += "E_CTRL_TYPE_SENTINEL";
                    break;
                case E_CTRL_TYPE_START:
                    wf += "E_CTRL_TYPE_START";
                    break;
                case E_CTRL_TYPE_PAUSE:
                    wf += "E_CTRL_TYPE_PAUSE";
                    break;
                case E_CTRL_TYPE_RESUME:
                    wf += "E_CTRL_TYPE_RESUME";
                    break;
                case E_CTRL_TYPE_FINISH:
                    wf += "E_CTRL_TYPE_FINISH";
                    break;
                default:
                    AERROR << "GetWorkFlow()->ctrl() NG!";
                    break;
                }

#ifdef CAMB_PKG_DBG
                //AINFO << "UpdateRobotWorkingFlow: {" <<
                    //wf << "}";
#endif
                setOutput("working_flow", wf);

                return NodeStatus::SUCCESS;
            };

            virtual NodeStatus onRunning() override {
#ifdef CAMB_PKG_DBG
                AINFO << "UpdateRobotWorkingFlow on running";
#endif
                return NodeStatus::SUCCESS;
            }

            void onHalted() override {
#ifdef CAMB_PKG_DBG
                AINFO << "UpdateRobotWorkingFlow on halt";
#endif
            }

        private:
            TaskSched* sch_ = nullptr;
    };

    class HandleRobotWorkingFlow : public StatefulActionNode {
        public:
            HandleRobotWorkingFlow(const std::string& name,
                    const NodeConfig& config) :
                StatefulActionNode(name, config) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "[construct] handle robot working flow: " << name;
#endif
                }

            static PortsList providedPorts() {
#ifdef CAMB_PKG_DBG
                AINFO << "ProvidedPorts handle robot working flow";
#endif
                return { InputPort<SwitchWorkFlow>("working_flow") };
            }

            virtual NodeStatus onStart() override {
#ifdef CAMB_PKG_DBG
                //AINFO << "HandleRobotWorkingFlow on start";
#endif
                return NodeStatus::RUNNING;
            };

            virtual NodeStatus onRunning() override {
                Expected<SwitchWorkFlow> msg =
                    getInput<SwitchWorkFlow>("working_flow");

                if (!msg->ctl_work_flow()) {
#ifdef CAMB_PKG_DBG
                    //AINFO << "HandleRobotWorkingFlow, " <<
                        //"ignore OK!";
#endif
                    return NodeStatus::SUCCESS;
                }

                switch (msg->ctrl()) {
                case E_CTRL_TYPE_PAUSE:
#ifdef CAMB_PKG_DBG
                    //AINFO << "HandleRobotWorkingFlow, " <<
                        //"pause";
#endif
                    break;
                case E_CTRL_TYPE_RESUME:
#ifdef CAMB_PKG_DBG
                    //AINFO << "HandleRobotWorkingFlow, " <<
                        //"resume";
#endif
                    break;
                case E_CTRL_TYPE_FINISH:
#ifdef CAMB_PKG_DBG
                    //AINFO << "HandleRobotWorkingFlow, " <<
                        //"finish";
#endif
                    break;
                default:
                    AERROR << "SwitchWorkFlow.ctrl NG: " <<
                        msg->ctrl();
                    break;
                }

                return NodeStatus::SUCCESS;
            }

            void onHalted() override {
#ifdef CAMB_PKG_DBG
                AINFO << "HandleRobotWorkingFlow on halt";
#endif
            }
    };

}; //brain
}; //camb
}; //mstf

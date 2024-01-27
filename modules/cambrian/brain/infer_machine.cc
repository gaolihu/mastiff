#include "cyber/common/macros.h"

#include "modules/cambrian/brain/infer_machine.h"

namespace mstf {
namespace camb {
namespace brain {

    InferMachine::InferMachine() {
#ifdef CAMB_PKG_DBG
        AINFO << "InferMachine construct";
#endif
        cache_pool_ = std::make_shared<MessagePool>();
        sm_mode_stat_ = std::make_shared<RobotModeStatus>();
    }

    InferMachine::~InferMachine() {
#ifdef CAMB_PKG_DBG
        AINFO << "InferMachine de-construct";
#endif
    }

	void InferMachine::Init() {
#ifdef CAMB_PKG_DBG
		AINFO << "init InferMachine";
#endif
        last_status_ = status_;
        last_mode_ = mode_;

        status_ = E_STATUS_NOWORK;
        status_pending_ = false;

        mode_ = E_MODE_STAND_BY;
        mode_pending_ = false;

        ShowModeStatus();
    }

	void InferMachine::Start() {
#ifdef CAMB_PKG_DBG
		AINFO << "start InferMachine";
#endif
        RequestChassisInfo();
        UpdataInferMachine();
    }

	void InferMachine::Stop() {
#ifdef CAMB_PKG_DBG
		AINFO << "stop InferMachine";
#endif
    }

	void InferMachine::PushPowerKey(const
            EE_CMD_FROM_WHO who) {
#ifdef CAMB_PKG_DBG
		AINFO << "power key short";
#endif
        DriveRobotWorking(E_CTRL_TYPE_START,
                E_KEY_POWER,
                E_CLEAN_GLOBAL, who);
    }

	void InferMachine::PushPowerKeyLong(const
            EE_CMD_FROM_WHO who) {
#ifdef CAMB_PKG_DBG
		AINFO << "power key long";
#endif
        DriveRobotWorking(E_CTRL_TYPE_START,
                E_KEY_DOCK,
                E_CLEAN_GLOBAL, who);
    }

    //TODO
	void InferMachine::PushHomeKey(const
            EE_CMD_FROM_WHO who) {
#ifdef CAMB_PKG_DBG
		AINFO << "home key short";
#endif
        DriveRobotWorking(E_CTRL_TYPE_START,
                E_KEY_DOCK,
                E_CLEAN_GLOBAL, who);
    }

	void InferMachine::PushHomeKeyLong(const
            EE_CMD_FROM_WHO who) {
#ifdef CAMB_PKG_DBG
		AINFO << "home key long";
#endif
        DriveRobotWorking(E_CTRL_TYPE_START,
                E_KEY_DOCK,
                E_CLEAN_GLOBAL, who);
    }

	void InferMachine::EventOnDock(const bool dock) {
        last_status_ = status_;
        last_mode_ = mode_;

        if (dock) {
            status_ = E_STATUS_NOWORK;
            if (full_)
                mode_ = E_MODE_CHARGE_END;
            else
                mode_ = E_MODE_CHARGING;
        } else {
            status_ = E_STATUS_NOWORK;
            mode_ = E_MODE_STAND_BY;
            //drive docking? TODO
        }

        exec_ = false;
        job_new_ = false;
#ifdef CAMB_PKG_DBG
        ShowModeStatus();
#endif
        UpdataInferMachine();
    }

	void InferMachine::EventChargeFull(const bool full) {
        last_status_ = status_;
        last_mode_ = mode_;

        if (full) {
            full_ = true;
            status_ = E_STATUS_NOWORK;
            mode_ = E_MODE_CHARGE_END;
        } else {
            full_ = false;
            if (mode_ == E_MODE_CHARGE_END)
                mode_ = E_MODE_CHARGING;
        }

        exec_ = false;
        job_new_ = false;
#ifdef CAMB_PKG_DBG
        ShowModeStatus();
#endif
        UpdataInferMachine();
    }

    std::shared_ptr<Message> InferMachine::ExposeStatMode() {
#ifdef CAMB_PKG_DBG
		AINFO << "expose status & mode" << sm_mode_stat_ <<
            ", use_count: " << sm_mode_stat_.use_count() <<
            ", content p: " << sm_mode_stat_.get() <<
            ", is unique: " << (sm_mode_stat_.unique() ? "YES" : "NO");
#endif
        if (sm_mode_stat_.get() != nullptr) {
            return sm_mode_stat_;
        } else {
            return nullptr;
        }
    }

    std::string InferMachine::GetDriveString(
            const EE_CMD_FROM_WHO who,
            const EE_ACT_KEY_TYPE act,
            const EE_CONTROL_TYPE ctrl) const {
		std::string str = "";
        std::ostringstream os;

        switch (who) {
        case E_CMD_FROM_SENTINEL:
            str = "Drive: { from: Nul? ";
            break;
        case E_CMD_FROM_IOT:
            str = "Drive: { from: IOT, ";
            break;
        case E_CMD_FROM_RSV:
            str = "Drive: { from: APPOINTMENT, ";
            break;
        case E_CMD_FROM_KEY:
            str = "Drive: { from: KEY, ";
            break;
        case E_CMD_FROM_H5:
            str = "Drive: { from: WEB H5, ";
            break;
        default:
            os << "Drive: { from UNKNOWN(";
			os << who;
			os << "), ";
            str = os.str();
            break;
        }

        switch (act) {
        case E_KEY_SENTINEL:
            str += "??: Nul? ";
            break;
        case E_KEY_POWER:
            str += "key: power, ";
            break;
        case E_KEY_DOCK:
            str += "key: dock, ";
            break;
        default:
            os << str;
            os << "key: unknown(";
			os << act;
			os << "), ";
            str = os.str();
            break;
        }

        switch (ctrl) {
        case E_CTRL_TYPE_SENTINEL:
            str += "L}";
            break;
        case E_CTRL_TYPE_START:
            str += "control: start }";
            break;
        case E_CTRL_TYPE_PAUSE:
            str += "control: pause }";
            break;
        case E_CTRL_TYPE_RESUME:
            str += "control: continue }";
            break;
        case E_CTRL_TYPE_FINISH:
            str += "control: terminate }";
            break;
        case E_CTRL_TYPE_RESTART:
            str += "control: restart-TODO }";
            break;
        default:
            os << str;
            os << "control unknown(";
			os << ctrl;
			os << ") }";
            str = os.str();
            break;
        }

        return str;
    }

	std::string InferMachine::GetStatString(const int stat) const {
		std::string str = "";
        std::ostringstream os;
		EE_ROBOT_STATUS t_stat;

		if (stat == -1)
			t_stat = status_;
		else
			t_stat = (EE_ROBOT_STATUS)stat;

		switch (t_stat) {
		case E_STATUS_NOWORK:
			str = "nowork";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_STATUS_PAUSING:
			str = "pausing";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_STATUS_WORKING:
			str = "working";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_STATUS_STOPWORK:
			str = "stopping";
			str += status_pending_ ? ":P" : ":S";
			break;
		default:
			os << stat;
			os << " stat-null";
			os << (status_pending_ ? ":P" : ":S");
			return os.str();
		}

		return str;
	}

	std::string InferMachine::GetStatStrLast(int stat) const {
		EE_ROBOT_STATUS t_stat;
		std::string str = "";
		std::ostringstream os;

		if (stat == -1)
			t_stat = last_status_;
		else
			t_stat = (EE_ROBOT_STATUS)stat;

		switch (t_stat) {
		case E_STATUS_NOWORK:
            str = "nowork";
            break;
		case E_STATUS_PAUSING:
            str = "pausing";
            break;
		case E_STATUS_WORKING:
            str = "working";
            break;
		case E_STATUS_STOPWORK:
            str = "stopping";
            break;
		default:
			//os << stat;
			os << "stat-null";
			return os.str();
		}

		return str;
	}

	std::string InferMachine::GetModeString(int mode) const {
		EE_ROBOT_MODE t_mode;
		std::string str = "";
        std::ostringstream os;

		if (mode == -1)
			t_mode = mode_;
		else
			t_mode = (EE_ROBOT_MODE)mode;


		switch (t_mode) {
		case E_MODE_AUTO_CLEAN:
			str = "auto-clean";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_AREA_CLEAN:
			str = "area-clean";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_ROOM_CLEAN:
			str = "room-clean";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_POINT_CLEAN:
			str = "point-clean";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_REMOTE_CLEAN:
			str = "remote-ctrl";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_LOCAL_CLEAN:
			str = "local-clean";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_TRACK_CLEAN:
			str = "tracing-track";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_STAND_BY:
			str = "no-job";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_DOCKING:
			str = "docking";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_CHARGING:
			str = "charging";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_CHARGE_END:
			str = "finish-charge";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_IN_FAULT:
			str = "in-fault";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_DORMANT:
			str = "dormant";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_ALONG_WALL:
			str = "along-wall";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_SWIFT_MAP:
			str = "swift-map";
			str += status_pending_ ? ":P" : ":S";
			break;
		case E_MODE_RELOCATION:
			str = "relocation";
			str += status_pending_ ? ":P" : ":S";
			break;
		default:
			os << mode;
			os << "mode-null";
			os << (status_pending_ ? ":P" : ":S");
			return os.str();
		}

		return str;
	}

	std::string InferMachine::GetModeStrLast(int mode) const {
		EE_ROBOT_MODE t_mode;
		std::string str = "";
        std::ostringstream os;

		if (mode == -1)
			t_mode = last_mode_;
		else
			t_mode = (EE_ROBOT_MODE)mode;

		switch (t_mode) {
		case E_MODE_AUTO_CLEAN:
            str = "auto-clean";
            break;
		case E_MODE_AREA_CLEAN:
            str = "area-clean";
            break;
		case E_MODE_ROOM_CLEAN:
            str = "room-clean";
            break;
		case E_MODE_POINT_CLEAN:
            str = "point-clean";
            break;
		case E_MODE_REMOTE_CLEAN:
            str = "remote-ctrl";
            break;
		case E_MODE_LOCAL_CLEAN:
            str = "local-clean";
            break;
		case E_MODE_STAND_BY:
            str = "standby";
            break;
		case E_MODE_DOCKING:
            str = "docking";
            break;
		case E_MODE_CHARGING:
            str = "charging";
            break;
		case E_MODE_CHARGE_END:
            str = "finish-charging";
            break;
		case E_MODE_IN_FAULT:
            str = "fall fault";
            break;
		case E_MODE_DORMANT:
            str = "dormant";
            break;
		case E_MODE_ALONG_WALL:
            str = "along-wall";
            break;
		case E_MODE_SWIFT_MAP:
            str = "swift-mapping";
            break;
		case E_MODE_RELOCATION:
            str = "relocation";
            break;
		default:
			//os << mode;
			os << "mode-nul";
			return os.str();
		}

		return str;
	}

	void InferMachine::FixModeStatus() {
        AWARN << "fix robot mode & status";
        if (!status_pending_ && mode_pending_)
            return;

        status_ = E_STATUS_NOWORK;
        mode_ = E_MODE_STAND_BY;

        status_pending_ = false;
        mode_pending_ = false;

        //1, check chass status, TODO
        //2, stop all work, TODO
    }

	void InferMachine::ShowModeStatus(
            const EE_CMD_FROM_WHO who,
            const EE_ACT_KEY_TYPE key,
            const EE_CONTROL_TYPE ctrl) const {
		//std::string action = GetDriveString(who, key, ctrl);

		AINFO << "{" << GetModeStrLast() <<
            ", " << GetStatStrLast() <<
            //" } Action: { " << action <<
            " -> " << GetModeString() <<
            ", " << GetStatString() <<
            "} {session: " <<
            session_.active_session_id().value() <<
            ", last: " <<
            session_.last_session_id().value() <<
            "}";
	}

    std::string InferMachine::ShowRobotDriving(
            const EE_CONTROL_TYPE ctrl,
            const EE_ACT_KEY_TYPE act,
            const EE_CLEAN_MODE mode,
            const EE_CMD_FROM_WHO who) const {
		std::string str = "";
        std::ostringstream os;

		std::string s_who_action =
            GetDriveString(who, act, ctrl);

        switch (mode) {
        case E_CLEAN_GLOBAL:
            str += ", Mode: { clean mode: global },";
            break;
        case E_CLEAN_AREA:
            str += ", Mode: { clean mode: area },";
            break;
        case E_CLEAN_ROOM:
            str += ", Mode: { clean mode: smart },";
            break;
        case E_CLEAN_POINT:
            str += ", Mode: { clean mode: point },";
            break;
        case E_CLEAN_REMOTE:
            str += ", Mode: { clean mode: remote },";
            break;
        case E_CLEAN_LOCAL:
            str += ", Mode: { clean mode: local },";
            break;
        case E_CLEAN_TRACK:
            str += ", Mode: { clean mode: track },";
            break;
        default:
            os << ", Mode: { clean mode: unknown: ";
			os << act;
			os << " },";
            str = os.str();
            break;
        }

        //AINFO << "Driving robot: " << s_who_action << " <<< " << str;
        return s_who_action + str;
    }

    std::string InferMachine::ShowRobotJobExec() const {
        std::string str = "";
        if (job_new_) {
            str = " Infer Job: { new: YES";
        } else
            str = " Infer Job: { new: No";

        if (exec_) {
            str += ", exec: YES }";
        } else {
            str += ", exec: NO }";
        };
        return str;
    }

    //core infer machine
	bool InferMachine::DriveRobotWorking(
            const EE_CONTROL_TYPE ctrl,
            const EE_ACT_KEY_TYPE o_act,
            const EE_CLEAN_MODE mode,
            const EE_CMD_FROM_WHO who) {
        if (IsPending())
            FixModeStatus();

        EE_ACT_KEY_TYPE act = o_act;
        if (o_act != E_KEY_POWER ||
                o_act != E_KEY_DOCK) {
            act = E_KEY_POWER;
        }

        work_mode_ = mode;

        switch (ctrl) {
        case E_CTRL_TYPE_START: {
            //start to work
            if (status_ == E_STATUS_NOWORK) {
                //1, nowork -> work
                last_status_ = E_STATUS_NOWORK;
                last_mode_ = mode_;

                status_ = E_STATUS_WORKING;
                mode_ = RobotModeDeduction(act, mode);

                exec_ = true;
                job_new_ = true;
#ifdef CAMB_PKG_DBG
                AINFO << "[nowork --> start] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
            } else if (status_ == E_STATUS_PAUSING) {
                //2, pausing -> work
                if (RobotModeDeduction(act, mode) == mode_) {
                    //start a same new job, ignore
                    AERROR << "already in pausing, mode: " <<
                        GetModeString() << ", but start again!";
#ifdef CAMB_PKG_DBG
                    AINFO << "[pausing --> start] " <<
                        ShowRobotDriving(ctrl, act, mode, who) <<
                        ShowRobotJobExec();
#endif
                    return false;
                } else {
                    //switch to new mode, OK
                    last_status_ = E_STATUS_PAUSING;
                    last_mode_ = mode_;

                    status_ = E_STATUS_WORKING;
                    mode_ = RobotModeDeduction(act, mode);

                    exec_ = true;
                    job_new_ = true;
#ifdef CAMB_PKG_DBG
                    AINFO << "[pausing --> start] " <<
                        ShowRobotDriving(ctrl, act, mode, who) <<
                        ShowRobotJobExec();
#endif
                }
            } else if (status_ == E_STATUS_WORKING) {
                //3, working -> work
                if (RobotModeDeduction(act, mode) == mode_) {
                    AERROR << "already in working, mode: " <<
                        GetModeString() << ", but start again!";
#ifdef CAMB_PKG_DBG
                    AINFO << "[working --> start] " <<
                        ShowRobotDriving(ctrl, act, mode, who) <<
                        ShowRobotJobExec();
#endif
                    return false;
                } else {
                    //switch to new mode
                    last_status_ = E_STATUS_WORKING;
                    last_mode_ = mode_;

                    status_ = E_STATUS_WORKING;
                    mode_ = RobotModeDeduction(act, mode);

                    exec_ = true;
                    job_new_ = true;
#ifdef CAMB_PKG_DBG
                    AINFO << "[working --> start] " <<
                        ShowRobotDriving(ctrl, act, mode, who) <<
                        ShowRobotJobExec();
#endif
                }
            } else if (status_ == E_STATUS_STOPWORK) {
                //4, stopping -> work
                last_status_ = E_STATUS_STOPWORK;
                last_mode_ = mode_;

                status_ = E_STATUS_WORKING;
                mode_ = RobotModeDeduction(act, mode);

                exec_ = true;
                job_new_ = true;
#ifdef CAMB_PKG_DBG
                AINFO << "[stopping --> start] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
            }
        }
            break;

        case E_CTRL_TYPE_PAUSE: {
            //pause work
            if (status_ == E_STATUS_NOWORK) {
                //1, nowork -> pause
                AERROR << "nowork, mode: " <<
                    GetModeString() << ", but pause!";
#ifdef CAMB_PKG_DBG
                AINFO << "[nowork --> pause] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            } else if (status_ == E_STATUS_PAUSING) {
                //2, pausing -> pause
                AERROR << "already in pausing, mode: " <<
                    GetModeString() << ", but pause!";
#ifdef CAMB_PKG_DBG
                AINFO << "[pausing --> pause] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            } else if (status_ == E_STATUS_WORKING) {
                //3, working -> pause
                if (RobotModeDeduction(act, mode) != mode_) {
                    AERROR << "not working at the same mode: " <<
                        GetModeString() << ", ignore pause!";
#ifdef CAMB_PKG_DBG
                    AINFO << "[working --> pause] " <<
                        ShowRobotDriving(ctrl, act, mode, who) <<
                        ShowRobotJobExec();
#endif
                    return false;
                } else {
                    last_status_ = E_STATUS_WORKING;
                    last_mode_ = mode_;

                    status_ = E_STATUS_PAUSING;
                    mode_ = RobotModeDeduction(act, mode);

                    //need exec task
                    exec_ = true;
#ifdef CAMB_PKG_DBG
                    AINFO << "[working --> pause] " <<
                        ShowRobotDriving(ctrl, act, mode, who) <<
                        ShowRobotJobExec();
#endif
                }
            } else if (status_ == E_STATUS_STOPWORK) {
                //4, stopping -> pause
                AERROR << "stoping, mode: " <<
                    GetModeString() << ", but pause!";
#ifdef CAMB_PKG_DBG
                AINFO << "[stopping --> pause] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            }
        }
            break;

        case E_CTRL_TYPE_RESUME: {
            //continue work
            if (status_ == E_STATUS_NOWORK) {
                //1, nowork -> continue, ignore
                AERROR << "nowork, mode: " <<
                    GetModeString() << ", ignore continue!";
#ifdef CAMB_PKG_DBG
                AINFO << "[nowork --> continue] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            } else if (status_ == E_STATUS_PAUSING) {
                //2, pausing -> continue
                if (RobotModeDeduction(act, mode) != mode_) {
                    //not at the same mode, ignore go on
                    AERROR << "not working at the same mode: " <<
                        GetModeString() << ", ignore continue!";
#ifdef CAMB_PKG_DBG
                    AINFO << "[pausing --> continue] " <<
                        ShowRobotDriving(ctrl, act, mode, who) <<
                        ShowRobotJobExec();
#endif
                } else {
                    last_status_ = E_STATUS_PAUSING;
                    last_mode_ = mode_;

                    status_ = E_STATUS_WORKING;
                    mode_ = RobotModeDeduction(act, mode);//?

                    exec_ = true;
#ifdef CAMB_PKG_DBG
                AINFO << "[pausing --> continue] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                }
            } else if (status_ == E_STATUS_WORKING) {
                //3, working -> continue, ignore
                AERROR << "working, mode: " <<
                    GetModeString() << ", ignore continue!";
#ifdef CAMB_PKG_DBG
                AINFO << "[working --> continue] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            } else if (status_ == E_STATUS_STOPWORK) {
                //4, stopping -> continue, ignore
                AERROR << "stopping, mode: " <<
                    GetModeString() << ", ignore continue!";
#ifdef CAMB_PKG_DBG
                AINFO << "[stopping --> continue] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            }
        }
            break;

        case E_CTRL_TYPE_FINISH: {
            //terminate work
            if (status_ == E_STATUS_NOWORK) {
                //1, nowork -> end work, ignore
                AERROR << "nowork, mode: " <<
                    GetModeString() << ", ignore finish!";
#ifdef CAMB_PKG_DBG
                AINFO << "[nowork --> end work] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            } else if (status_ == E_STATUS_PAUSING) {
                //2, pausing -> end work
                last_status_ = E_STATUS_PAUSING;
                last_mode_ = mode_;

                status_ = E_STATUS_STOPWORK;
                mode_ = E_MODE_STAND_BY;

                exec_ = true;
#ifdef CAMB_PKG_DBG
                AINFO << "[pausing --> end work] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
            } else if (status_ == E_STATUS_WORKING) {
                //3, working -> end work
                last_status_ = E_STATUS_WORKING;
                last_mode_ = mode_;

                status_ = E_STATUS_STOPWORK;
                mode_ = E_MODE_STAND_BY;

                exec_ = true;
#ifdef CAMB_PKG_DBG
                AINFO << "[working --> end work] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
            } else if (status_ == E_STATUS_STOPWORK) {
                //4, stopping -> end work
                AERROR << "stopping, mode: " <<
                    GetModeString() << ", ignore finish!";
#ifdef CAMB_PKG_DBG
                AINFO << "[stopping --> end work] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            }
        }
            break;

        case E_CTRL_TYPE_RESTART: {
            //restart, TODO
            if (status_ == E_STATUS_NOWORK) {
#ifdef CAMB_PKG_DBG
                AINFO << "[nowork --> restart] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            } else if (status_ == E_STATUS_PAUSING) {
#ifdef CAMB_PKG_DBG
                AINFO << "[pausing --> restart] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            } else if (status_ == E_STATUS_WORKING) {
#ifdef CAMB_PKG_DBG
                AINFO << "[working --> restart] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            } else if (status_ == E_STATUS_STOPWORK) {
#ifdef CAMB_PKG_DBG
                AINFO << "[stopping --> restart] " <<
                    ShowRobotDriving(ctrl, act, mode, who) <<
                    ShowRobotJobExec();
#endif
                return false;
            }
        }

        default:
            AERROR << "control type error: " << ctrl;

            status_pending_ = true;
            mode_pending_ = true;

            return false;
        }
#ifdef CAMB_PKG_DBG
        //ShowModeStatus(who, act, ctrl);
        ShowModeStatus();
#endif
        UpdataInferMachine();

        return true;
    }

    void InferMachine::UpdataInferMachine() {
#ifdef CAMB_PKG_DBG
        AINFO << "update mode & status";
#endif
        sm_mode_stat_->set_status(status_);
        sm_mode_stat_->set_mode(mode_);
        sm_mode_stat_->set_error(error_);
        sm_mode_stat_->set_battery(battery_);
        sm_mode_stat_->set_fan(fan_);
        sm_mode_stat_->set_flow(flow_);
        sm_mode_stat_->set_boost(boost_);
        sm_mode_stat_->set_exec(exec_);
        sm_mode_stat_->set_mission_new(job_new_);

        PlanWorkingFlowPath();
    }

    bool InferMachine::DriveRobotWorking(const
            MissionCommand& mc) {
#if 0
        AINFO << "drive robot mission, command:\n" <<
            mc.DebugString();
#endif
        if (!mc.has_mzp()) {
            AWARN << "invalid mission:\n" <<
                mc.DebugString();
            return false;
        }

        bool ret = DriveRobotWorking(mc.cmd(), mc.key(),
                mc.mode(), mc.user());

        //for remote mzp update
        if (ret)
            //drive working OK
            UpdateMultiZonesParam();

        return ret;
    }

    bool InferMachine::DriveRobotShiftWork(const
            std::shared_ptr<Message>& msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "InferMachine switch robot work status";
#endif
        //if (MessageTrait::Instance()->
                //MsgTypeUpModeStatus<Message>(msg)) {
        if (1) {
            DriveRobotWorking(std::dynamic_pointer_cast<
                    RobotModeStatus>(msg)->task_shift().ctrl(),
                    E_KEY_ACT_NULL, work_mode_,
                    E_CMD_FROM_LOGIC);
            PlanWorkingFlowPath();
            return 0;
        } else {
            AERROR << "shift fault?!";
        }

        return -1;
    }

    void InferMachine::UpdataBattery(const uint32_t bat) {
#ifdef CAMB_PKG_DBG
        //AINFO << "InferMachine update battery: " << bat;
#endif
        battery_ = bat;
        if (bat != 100)
            full_ = false;
        PlanWorkingFlowPath();
    }

    void InferMachine::UpdataFan(const uint32_t fan) {
#ifdef CAMB_PKG_DBG
        AINFO << "InferMachine update fan: " << fan;
#endif
        fan_ = fan;
        PlanWorkingFlowPath();
    }

    void InferMachine::UpdataFlow(const uint32_t flow) {
#ifdef CAMB_PKG_DBG
        AINFO << "InferMachine update flow: " << flow;
#endif
        flow_ = flow;
        PlanWorkingFlowPath();
    }

    void InferMachine::UpdataBoost(const uint32_t boost) {
#ifdef CAMB_PKG_DBG
        AINFO << "InferMachine update boost: " << boost;
#endif
        boost_ = boost;
        PlanWorkingFlowPath();
    }

} //namespace brain
} //namespace camb
} //namespace mstf

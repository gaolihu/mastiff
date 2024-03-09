#include "modules/cambrian/brain/abstract_bot.h"

namespace mstf {
namespace camb {
namespace brain {

    AbstractBot::AbstractBot(const
            std::shared_ptr<CambrianConfig>& cc) :
        BotItfImpl(cc) {
#ifdef CAMB_PKG_DBG
        AINFO << "Abstract ROBOT construct";
#endif
        //handle query
        RegisterRemoteMsgHandle<QuerySystemInfo>
            (&AbstractBot::HandleQuerySystemInfo);
        RegisterRemoteMsgHandle<QueryPeriphInfo>
            (&AbstractBot::HandleQueryPeriphInfo);
        RegisterRemoteMsgHandle<QueryVolatileInfo>
            (&AbstractBot::HandleQueryVolatileInfo);
        RegisterRemoteMsgHandle<QueryChassisRaw>
            (&AbstractBot::HandleQueryChassisRaw);
        RegisterRemoteMsgHandle<QueryModeStatus>
            (&AbstractBot::HandleQueryModeStatus);
        RegisterRemoteMsgHandle<QueryCoverTrack>
            (&AbstractBot::HandleQueryCoverTrack);
        RegisterRemoteMsgHandle<QueryLocArray>
            (&AbstractBot::HandleQueryLocArray); //remote query
        RegisterRemoteMsgHandle<LocationArray>
            (&AbstractBot::HandleQueryLocArray); //local report
        RegisterRemoteMsgHandle<QueryGridMap>
            (&AbstractBot::HandleQueryGridMap);
        RegisterRemoteMsgHandle<QueryMapStorage>
            (&AbstractBot::HandleQueryMapStorage);
        RegisterRemoteMsgHandle<QueryMapID>
            (&AbstractBot::HandleQueryMapID);
        RegisterRemoteMsgHandle<QueryZones>
            (&AbstractBot::HandleQueryZones);

        //handle commands
        RegisterRemoteMsgHandle<MissionCommand>
            (&AbstractBot::HandleMissionCommand);

        //chassis control
        RegisterRemoteMsgHandle<MiscChassisCtrl>
            (&AbstractBot::HandleChassisCtrl);

        //??
        /*
        RegisterRemoteMsgHandle<SettingBother>
            (&AbstractBot::HandleSettingBother);
        RegisterRemoteMsgHandle<SettingConsumable>
            (&AbstractBot::HandleSettingConsumable);
                */
#ifdef CAMB_PKG_DBG
        AINFO << "Abstract robot construct OK";
#endif
    }

    AbstractBot::~AbstractBot() {
#ifdef CAMB_PKG_DBG
        AINFO << "AbstractBot de-construct";
#endif
    }

    //common
    void AbstractBot::Init(const
            BaseMsgCaller& ex) {
#ifdef CAMB_PKG_DBG
        AINFO << "AbstractBot Initialize";
#endif
        BotItfImpl::Init(ex);
    }

    void AbstractBot::Start() {
#ifdef CAMB_PKG_DBG
        AINFO << "AbstractBot Start";
#endif
        BotItfImpl::Start();
    }

    void AbstractBot::Stop() {
#ifdef CAMB_PKG_DBG
        AINFO << "AbstractBot Stop";
#endif
        BotItfImpl::Stop();
    }

    void AbstractBot::Resume() {
#ifdef CAMB_PKG_DBG
        AINFO << "AbstractBot Resume";
#endif
        BotItfImpl::Resume();
    }

    void AbstractBot::Close() {
#ifdef CAMB_PKG_DBG
        AINFO << "AbstractBot Close";
#endif
        BotItfImpl::Close();
    }

    template <typename MessageT>
    void AbstractBot::BotItfImpl::RegisterRemoteMsgHandle(
            HandleRemoteMessage handle_s) {
        remote_msg_handle_pair_[
            cyber::message::GetMessageName<MessageT>()] = handle_s;

        AINFO << "[" <<
            remote_msg_handle_pair_.size() <<
            "] remote handle: " <<
            cyber::message::GetMessageName<MessageT>();
    }

    ////////////////////// HANDLES //////////////////////
    int AbstractBot::BotItfImpl::HandleQuerySystemInfo(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQuerySystemInfo: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        auto sys = sys_processor_->DequeueMessage<
            SystemInformation>();
        if (sys == nullptr) {
            AWARN << "message pool empty sysinfo, checkout from db!";
            sys = database_->DbExposeSystemInfo();
            if (sys == nullptr) {
                AWARN << "database sys info null!";
                return -1;
            }

            sys_processor_->SetSystemInfoPtr(sys);
        }

        visual_->UpdateRemoteSystemInfo(sys);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryPeriphInfo(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryPeriphInfo: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto ppr = sys_processor_->DequeueMessage<PeriphInformation>();
        if (ppr == nullptr) {
            AWARN << "message pool empty periph data!";
            return -1;
        }

        visual_->UpdateRemotePeriphInfo(ppr);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryVolatileInfo(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        AINFO_EVERY(20) << "HandleQueryVolatileInfo: " <<
            (msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto vola = sys_processor_->DequeueMessage<
            VolatileInformation>();
        if (vola == nullptr) {
            AWARN << "message pool empty vola , checkout from db!";
            vola = database_->DbExposeVolatileInfo();
            if (vola == nullptr) {
                AWARN << "database volatile info null!";
                return -1;
            }

            sys_processor_->SetVolatileInfoPtr(vola);
        }

//#ifdef CAMB_PKG_DBG
#if 0
        AINFO << "HandleQueryVolatileInfo: " <<
            ", volaptr: " << vola << ", sp: " << vola.get() <<
            ", use_count: " << vola.use_count() <<
            ", is unique: " << vola.unique();
#endif

        visual_->UpdateRemoteVolatileInfo(vola);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryChassisRaw(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryChassisRaw: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto chs = sys_processor_->DequeueMessage<ChassisRawData>();
        if (chs == nullptr) {
            AWARN << "message pool empty chassis raw!";
            return -1;
        }
        visual_->UpdateRemoteChassisRaw(chs);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryModeStatus(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryModeStatus: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto ms = sys_processor_->DequeueMessage<RobotModeStatus>();
        if (ms == nullptr) {
            AWARN << "message pool empty mode/stat, "
                "checkout from im!";
            ms = sys_processor_->ExposeStatMode();
            if (ms == nullptr) {
                AERROR << "infer machine mode stat null!";
                return -1;
            }

            sys_processor_->SetModeStatusPtr(ms);
        }
        visual_->UpdateRemoteModeStatus(ms);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryCoverTrack(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryCoverTrack: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto ct = sys_processor_->DequeueMessage<CleanCoverTrack>();
        if (ct == nullptr) {
            AWARN << "message pool empty path!";
            //TODO, load from database
            return -1;
        }
        visual_->UpdateRemoteCoverTrack(ct);

#endif
        return 0;
    }
    int AbstractBot::BotItfImpl::HandleQueryLocArray(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryLocArray: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        visual_->UpdateRemoteLocArray(msg);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryGridMap(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryGridMap: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto gm = sys_processor_->DequeueMessage<GlobalGridMap>();
        if (gm == nullptr) {
            //TODO, load from database
            AWARN << "message pool empty grid map!";
            return -1;
        }
        visual_->UpdateRemoteGridMap(gm);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryMapStorage(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryMapStorage: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto mps = sys_processor_->DequeueMessage<MapStorage>();
        if (mps == nullptr) {
            AWARN << "message pool empty stored maps!";
            //TODO, load from database
            return -1;
        }
        visual_->UpdateRemoteMapStorage(mps);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryMapID(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryMapID: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto id = sys_processor_->DequeueMessage<UniqueId>();
        if (id == nullptr) {
            AWARN << "message pool empty map id!";
            //TODO, load from database
            return -1;
        }
        visual_->UpdateRemoteMapID(id);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleQueryZones(
            std::shared_ptr<Message> msg) {
#if 0
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleQueryZones: " <<
            //(msg == nullptr ? "null" : msg->DebugString());
#endif
        //get PeriphInformation from message pool
        auto zones = sys_processor_->DequeueMessage<QueryZones>();
        if (zones == nullptr) {
            AWARN << "message pool empty zones , checkout from db!";
            zones = database_->DbExposeMltimZones();
            if (zones == nullptr) {
                AWARN << "database multi zones null!";
                return -1;
            }

            //update message pool
            sys_processor_->SetMultiZonesPtr(zones);

            //set bussines ptr, Implementor->Bs
            sys_processor_->SetMultiZonesParam(zones);

        }
        visual_->UpdateRemoteZones(zones);

#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleMissionCommand(
            const std::shared_ptr<Message> msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "HandleMissionCommand: " <<
#if 0
            msg->DebugString();
#else
            msg->GetTypeName();
#endif
#endif
        //update sql mzp
        auto mzp = (dynamic_cast<MissionCommand*>(msg.get()))->mzp();
        if (mzp.szp().size() && mzp.szp(0).plane_shape().size()) {
            //TODO
            //save multi-zones
            //database_->DbStoreMltimZones(mzp);
        }

        //drive robot
        return sys_processor_->InferMachine::DriveRobotWorking
            (*dynamic_cast<MissionCommand*>(msg.get()));
    }

    int AbstractBot::BotItfImpl::HandleAppointmentTask(
            std::shared_ptr<Message> msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "HandleAppointmentTask: " <<
            msg->DebugString();
#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleChassisCtrl(
            std::shared_ptr<Message> msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "HandleChassisCtrl:\n" <<
#if 1
            msg->DebugString();
#else
            "";
#endif
#endif
        return msg_publish_(msg);
    }

    int AbstractBot::BotItfImpl::HandleSettingBother(
            std::shared_ptr<Message> msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "HandleSettingBother: " <<
            msg->DebugString();
#endif
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleSettingConsumable(
            std::shared_ptr<Message> msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "HandleSettingConsumable: " <<
            msg->DebugString();
#endif
        return 0;
    }

    //service & client
    int AbstractBot::BotItfImpl::HandleDriveSlam(const
            std::shared_ptr<DriveSlam>& rqs,
            std::shared_ptr<SlamDriveAck>& rsp) {
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleDriveSlam:\n" <<
            //rqs->DebugString();
#endif
        //return frg_out_sdrv_rls_(rqs, rsp);
        return 0;
    }

    int AbstractBot::BotItfImpl::HandleDriveNavi(const
            std::shared_ptr<DriveNavigation>& rqs,
            std::shared_ptr<NavigationAck> rsp) {
#ifdef CAMB_PKG_DBG
        //AINFO << "HandleDriveNavi:\n" <<
            //rqs->DebugString();
#endif
        //return frg_out_navi_rls_(rqs, rsp);
        return 0;
    }

} //namespace brain
} //namespace camb
} //namespace mstf

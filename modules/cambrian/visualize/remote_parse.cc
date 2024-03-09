#include "modules/cambrian/visualize/remote_parse.h"

namespace mstf {
namespace camb {
namespace visual {

    //convert to string
    //upstream to be send to html web page
    std::shared_ptr<Message> RemoteParser::FormatNgxMsg(
            Message* msg, const std::string& str) {
#ifdef CAMB_PKG_DBG
        /*
        AINFO << "------>> FormatNgxMsg, msg type: " <<
            msg->GetTypeName();
            */
#else
        AWARN_EVERY(500) << "-----> FormatNgxMsg, msg type: " <<
            msg->GetTypeName();
#endif

        if (cyber::message::GetMessageName<SystemInformation>() ==
                msg->GetTypeName()) {
            return DoAckSystemInfo(msg);
        }

        /*
        if (cyber::message::GetMessageName<PeriphInformation>() ==
                msg->GetTypeName()) {
            return DoAckPeriphInfo(msg);
        }
        */

        if (cyber::message::GetMessageName<VolatileInformation>() ==
                msg->GetTypeName()) {
            return DoAckVolatileInfo(msg);
        }

        if (cyber::message::GetMessageName<ChassisRawData>() ==
                msg->GetTypeName()) {
            return DoAckChassisRaw(msg);
        }

        if (cyber::message::GetMessageName<RobotModeStatus>() ==
                msg->GetTypeName()) {
            return DoAckModeStatus(msg);
        }

        if (cyber::message::GetMessageName<CleanCoverTrack>() ==
                msg->GetTypeName()) {
            return DoAckCoverTrack(msg);
        }

        if (cyber::message::GetMessageName<LocationArray>() ==
                msg->GetTypeName()) {
            return DoAckLocArray(msg);
        }

        if (cyber::message::GetMessageName<GlobalGridMap>() ==
                msg->GetTypeName()) {
            return DoAckGridMap(msg);
        }

        if (cyber::message::GetMessageName<MapStorage>() ==
                msg->GetTypeName()) {
            return DoAckMapStorage(msg);
        }

        if (cyber::message::GetMessageName<UniqueId>() ==
                msg->GetTypeName()) {
            return DoAckMapID(msg);
        }

        if (cyber::message::GetMessageName<AppointmentTask>() ==
                msg->GetTypeName()) {
            return DoAckAppointment(msg);
        }

        AWARN << "formating nginx message: " << msg->GetTypeName() <<
            " error! origin:\n" << ", string: " << str;

        return nullptr;
    }

    //convert to proto
    //downstream to robot
    std::shared_ptr<Message> RemoteParser::DeformatNgxMsg(const std::string& str) {
#ifdef USE_SOCKETIO
        std::string sub_str = str.substr(WEB_ACT,
                str.find("HTTP/1.1") - strlen("HTTP/1.1"));
#endif

        FringeTransferMsg msg;
        msg.ParseFromArray(str.data(), str.size());

        if (msg.msg_cnt() <= 1)
            ds_msg_cnt_++;
        else
            ds_msg_cnt_ = msg.msg_cnt();

        ds_msg_bytes_ += str.size();


#ifdef CAMB_PKG_DBG
        AINFO << "<---- from web, parse msg, len: " << str.size() <<
            ", d-seq: " << ds_msg_cnt_ <<
            ", received " << (float)ds_msg_bytes_ / 1024.f << "Kb"
#if 0
            //for detailed debug
            << ", contents: " << "\n{\n" <<
            msg.DebugString() << "}";
#else
        ;
#endif
#endif

        //system info
        if (msg.has_q_sys()) {
            return DoQuerySystemInfo(nullptr);
        }

        //peripheral info
        /*
        else if (msg.has_q_ppi()) {
            return DoQueryPeriphInfo(
                    dynamic_cast<Message*>(msg.mutable_q_chsr()));
        }
        */

        //volatile info
        else if (msg.has_q_vli()) {
            return DoQueryVolatileInfo(nullptr);
        }

        //chassis raw info
        else if (msg.has_q_chsr()) {
            return DoQueryChassisRaw(dynamic_cast
                    <Message*>(msg.mutable_q_chsr()));
        }

        //mode & status
        else if (msg.has_q_ms()) {
            return DoQueryModeStatus(nullptr);
        }

        //coverd track
        else if (msg.has_q_ct()) {
            return DoQueryCoverTrack(dynamic_cast
                    <Message*>(msg.mutable_q_ct()));
        }

        //locs
        else if (msg.has_q_ct()) {
            return DoQueryLocArray(dynamic_cast
                    <Message*>(msg.mutable_q_loc()));
        }

        //grid map
        else if (msg.has_q_gm()) {
            return DoQueryGridMap(dynamic_cast
                    <Message*>(msg.mutable_q_mid()));
        }

        //storaged maps
        else if (msg.has_q_maps()) {
            return DoQueryMapStorage(nullptr);
        }

        //map ids
        else if (msg.has_q_mid()) {
            return DoQueryMapID(nullptr);
        }

        //appointment task
        else if (msg.has_q_am()) {
            return DoQueryAppointment(nullptr);
        }

        //zones
        else if (msg.has_q_z()) {
            return DoQueryZones(nullptr);
        }

        //...

        //robot mission
        else if (msg.has_command()) {
            return DoMissionCommand(dynamic_cast
                    <Message*>(msg.mutable_command()));
        }

        //appointment task setting
        else if (msg.has_a_am()) {
            AINFO << "setting appointment task message!";
            DoAppointmentTask(dynamic_cast
                    <Message*>(msg.mutable_a_am()));
        }

        //motion control
        else if (msg.has_mc()) {
            return DoMotionCtrl(dynamic_cast
                    <Message*>(msg.mutable_mc()));
        }

        //drive slam
        else if (msg.has_ds()) {
            return DoDriveSlam(dynamic_cast
                    <Message*>(msg.mutable_ds()));
        }

        //ignore motion control result from remote
        else if (msg.has_mc_result()) {
            AWARN << "ignore downstream mc result message!";
        }

        //ignore mission stat from remote
        else if (msg.has_mission_stat()) {
            AWARN << "ignore downstream mission stat message!!!";
        }

        //ignore finish info from remote
        else if (msg.has_finish_info()) {
            AWARN << "ignore downstream finish clean message!!!";
        }

        //ignore slam control result from remote
        else if (msg.has_a_ds()) {
            AWARN << "ignore downstream drive slam result message!";
        }

        //IGNORE raw messages frome remote
        /*
        else if (msg.has_a_ppi()) {
            AWARN << "ignore downstream periph message!";
        }*/ else if (msg.has_a_sys()) {
            AWARN << "ignore downstream system message!";
        } else if (msg.has_a_vli()) {
            AWARN << "ignore downstream volatile message!";
        } else if (msg.has_a_chsr()) {
            AWARN << "ignore downstream chassis raw message!";
        } else if (msg.has_a_ms()) {
            AWARN << "ignore downstream mode & status message!";
        } else if (msg.has_a_ct()) {
            AWARN << "ignore downstream path message!";
        } else if (msg.has_a_gm()) {
            AWARN << "ignore downstream grid map message!";
        } else if (msg.has_a_maps()) {
            AWARN << "ignore downstream stored maps message!";
        } else if (msg.has_a_mid()) {
            AWARN << "ignore downstream map id message!";
        } else {
            AWARN << "ignore msg undefined!!!";
        }

        return nullptr;
    }

    void RemoteParser::ToWebRaw(const std::string& in, std::string& out) {
#ifdef USE_SOCKETIO
        out = "";
        out += "HTTP/1.1 200 OK\r\n" \
                "Content-Type: text/html;charset = utf-8\r\n" \
                "Access-Control-Allow-Credentails: true\r\n" \
                "Access-Control-Allow-Origin:* \r\n" \
                "Connection: keep-alive\r\n" \
                "ConTent-Length:";

        out += std::to_string(in.length());

        out += "\n\n";
        out.append(in);
        out += "\r\n\r\n";
#else
        out = in;
#endif
    }

    std::shared_ptr<Message> RemoteParser::DoMissionCommand(Message* msg) {
#ifdef CAMB_PKG_DBG
        //AINFO << "receive remote clean mission!";
#endif
        return std::make_shared<MissionCommand>(
                *dynamic_cast<MissionCommand*>(msg));
    }

    std::shared_ptr<Message> RemoteParser::DoAppointmentTask(Message* msg) {
        return nullptr;
    }

    std::shared_ptr<Message> RemoteParser::DoChassisCtrl(const std::string&) {
        return nullptr;
    }

    std::shared_ptr<Message> RemoteParser::DoMotionCtrl(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //AINFO << "remote control robot";
        auto mc = std::make_shared<MotionCtrl>();
        if (dynamic_cast<MotionCtrl*>(msg) != nullptr) {
            mc->set_type(dynamic_cast<MotionCtrl*>(msg)->type());
            mc->set_direction(dynamic_cast<MotionCtrl*>(msg)->direction());
            /*
               mc->set_left();
               mc->set_right();
               mc->set_time();
               mc->set_distance();
               mc->set_delta_theta();
               mc->set_target_theta();
               mc->set_ack();
               */
        }
        return mc;
    }

    std::shared_ptr<Message> RemoteParser::DoDriveSlam(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);
        AINFO << "slam control";
        //auto csr = std::make_shared<ControlSlamRequest>();
        auto sc = std::make_shared<DriveSlam>();
        if (dynamic_cast<DriveSlam*>(msg) != nullptr) {
            //TODO
            // 
            //   
        }

        //return csr;
        return sc;
    }

    //----------------------------query----------------------------//
    std::shared_ptr<Message> RemoteParser::DoQuerySystemInfo(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query system info";
        return std::make_shared<QuerySystemInfo>();
    }

    /*
    std::shared_ptr<Message> RemoteParser::DoQueryPeriphInfo(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query peripheral info";
        auto ppi = std::make_shared<QueryPeriphInfo>();
        if (dynamic_cast<QueryPeriphInfo*>(msg) != nullptr)
            ppi->set_ppr_type(dynamic_cast<QueryPeriphInfo*>(msg)->ppr_type());
        return ppi;
    }
    */

    std::shared_ptr<Message> RemoteParser::DoQueryVolatileInfo(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query volatile info";
        return std::make_shared<QueryVolatileInfo>();
    }

    std::shared_ptr<Message> RemoteParser::DoQueryChassisRaw(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query chassis raw info";
        auto cr = std::make_shared<QueryChassisRaw>();
        if (dynamic_cast<QueryChassisRaw*>(msg) != nullptr)
            cr->set_chs_raw(dynamic_cast<QueryChassisRaw*>(msg)->chs_raw());
        return cr;
    }

    std::shared_ptr<Message> RemoteParser::DoQueryCoverTrack(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query covered path";
        if (dynamic_cast<QueryCoverTrack*>(msg) == nullptr) {
            AWARN << "remote query covered path error!";
            return nullptr;
        } else {
            auto cp = std::make_shared<QueryCoverTrack>();
            cp->set_p_id(dynamic_cast<QueryCoverTrack*>(msg)->p_id());
            cp->set_head(dynamic_cast<QueryCoverTrack*>(msg)->head());
            cp->set_tail(dynamic_cast<QueryCoverTrack*>(msg)->tail());
            return cp;
        }
    }

    std::shared_ptr<Message> RemoteParser::DoQueryLocArray(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query location";
        if (dynamic_cast<QueryLocArray*>(msg) == nullptr) {
            AWARN << "remote query covered path error!";
            return nullptr;
        } else {
            return std::make_shared<QueryCoverTrack>();
        }
    }

    std::shared_ptr<Message> RemoteParser::DoQueryGridMap(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query map id";
        auto gm = std::make_shared<QueryGridMap>();
        if (dynamic_cast<QueryGridMap*>(msg) != nullptr)
            gm->set_map_id(dynamic_cast<QueryGridMap*>(msg)->map_id());
        return gm;
    }

    std::shared_ptr<Message> RemoteParser::DoQueryModeStatus(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query volatile info";
        return std::make_shared<QueryModeStatus>();
    }

    std::shared_ptr<Message> RemoteParser::DoQueryMapStorage(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query stored maps";
        return std::make_shared<QueryMapStorage>();
    }

    std::shared_ptr<Message> RemoteParser::DoQueryMapID(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query map id";
        return std::make_shared<QueryMapID>();
    }

    std::shared_ptr<Message> RemoteParser::DoQueryAppointment(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query appointment task info";
        return std::make_shared<QueryAppointment>();
    }
    std::shared_ptr<Message> RemoteParser::DoQueryZones(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send in to robot
        //AINFO << "remote query appointment task info";
        return std::make_shared<QueryZones>();
    }
    //
    //----------------------------query----------------------------//

    //----------------------------ack----------------------------//
    //
    std::shared_ptr<Message> RemoteParser::DoAckSystemInfo(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_sys()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();

#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: system info, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

    /*
    std::shared_ptr<Message> RemoteParser::DoAckPeriphInfo(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_ppi()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();

#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: peripheral info, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }
    */

    std::shared_ptr<Message> RemoteParser::DoAckVolatileInfo(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_vli()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();

#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: volatile info, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

    std::shared_ptr<Message> RemoteParser::DoAckChassisRaw(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        ChassisRawData* tmp = dynamic_cast<ChassisRawData*>(msg);
        m->mutable_a_chsr()->CopyFrom(*tmp);
        us_msg_bytes_ += m->ByteSizeLong();

#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: chassis raw info, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb" <<
#if 1
            "";
#else
            ", msg:\n" << msg->DebugString();
#endif
#endif

        return m;
    }

    std::shared_ptr<Message> RemoteParser::DoAckCoverTrack(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_ct()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();
#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: covered poses, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

    std::shared_ptr<Message> RemoteParser::DoAckLocArray(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_loc()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();
#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: covered poses, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

    std::shared_ptr<Message> RemoteParser::DoAckGridMap(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_gm()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();

#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: map id, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

    std::shared_ptr<Message> RemoteParser::DoAckModeStatus(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_ms()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();
#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: mode & status info, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

    std::shared_ptr<Message> RemoteParser::DoAckMapStorage(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_maps()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();

#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: maps stored, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

    std::shared_ptr<Message> RemoteParser::DoAckMapID(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_mid()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();
#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: map id, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

    std::shared_ptr<Message> RemoteParser::DoAckAppointment(Message* msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        //send out to remote
        auto m = std::make_shared<FringeTransferMsg>();
        m->set_msg_cnt(++us_msg_cnt_);
        m->mutable_a_am()->CopyFrom(*msg);
        us_msg_bytes_ += m->ByteSizeLong();
#ifdef CAMB_PKG_DBG
        AWARN_EVERY(500) << "----> web: appointment task, u-seq: " << us_msg_cnt_ <<
            ", upload " << (float)us_msg_bytes_ / 1024.f << "Kb";
#endif

        return m;
    }

} //namespace visual
} //namespace camb
} //namespace mstf

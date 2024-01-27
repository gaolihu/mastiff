#pragma once

#include "cyber/common/log.h"
#include "cyber/cyber.h"

#include "modules/cambrian/proto/cambrian_arch.pb.h"
#include "modules/chassis/proto/external_msg.pb.h"

#define WEB_ACT 6

#define WEB_VALUE 13
#define SW_FLAG 18
#define SW_POST 22
#define WHEEL_DATA 25

#define CLIFF "CLIFF__"
#define WALL "WALL___"
#define TOF "TOF____"
#define WHEEL "WHEEL__"
#define FAN "FAN____"
#define BRUSH "BRUSH__"
#define PUMP "PUMP___"
#define CHARGE "CHARGE_"
#define POWER "POWER__"
#define BUMP "BUMP___"
#define MOP "MOP____"
#define LED "LED____"
#define CAMERA "CAMERA_"
#define USER "venus"
#define PASSWD "123456"

namespace mstf {
namespace camb {
namespace visual {

    using namespace apollo;
    using namespace google::protobuf;
    using namespace /*mstf::camb::*/proto;
    using namespace /*mstf::*/chss::proto;
    using namespace ventura::common_msgs::nav_msgs;
    using namespace ventura::common_msgs::visualization_msgs;

    //remote proto message handling
    using RemoteMessageHdl = std::function<void(const
            std::shared_ptr<Message>&)>;
    //remote raw message handling
    using RemoteStringHdl = std::function<int(const
            std::string&)>;

    class RemoteParser {
        public:
            RemoteParser() = default;
            ~RemoteParser() =default;

            // proto message -> web raw message
            std::shared_ptr<Message> FormatNgxMsg(Message*, const std::string&);
            // web raw message -> proto message
            std::shared_ptr<Message> DeformatNgxMsg(const std::string&);

        private:
            // web raw message formating
            void ToWebRaw(const std::string& in, std::string& out);

            //control
            std::shared_ptr<Message> DoMissionCommand(Message* = nullptr);
            std::shared_ptr<Message> DoAppointmentTask(Message* = nullptr);
            std::shared_ptr<Message> DoChassisCtrl(const std::string&);
            std::shared_ptr<Message> DoMotionCtrl(Message* = nullptr);
            std::shared_ptr<Message> DoDriveSlam(Message* = nullptr);

            //query
            std::shared_ptr<Message> DoQuerySystemInfo(Message* = nullptr);
            std::shared_ptr<Message> DoQueryPeriphInfo(Message* = nullptr);
            std::shared_ptr<Message> DoQueryVolatileInfo(Message* = nullptr);
            std::shared_ptr<Message> DoQueryChassisRaw(Message* = nullptr);
            std::shared_ptr<Message> DoQueryModeStatus(Message* = nullptr);
            std::shared_ptr<Message> DoQueryCoverTrack(Message* = nullptr);
            std::shared_ptr<Message> DoQueryLocArray(Message* = nullptr);
            std::shared_ptr<Message> DoQueryGridMap(Message* = nullptr);
            std::shared_ptr<Message> DoQueryMapStorage(Message* = nullptr);
            std::shared_ptr<Message> DoQueryMapID(Message* = nullptr);
            std::shared_ptr<Message> DoQueryAppointment(Message* = nullptr);
            std::shared_ptr<Message> DoQueryZones(Message* = nullptr);

            //ack
            std::shared_ptr<Message> DoAckSystemInfo(Message* = nullptr);
            std::shared_ptr<Message> DoAckPeriphInfo(Message* = nullptr);
            std::shared_ptr<Message> DoAckVolatileInfo(Message* = nullptr);
            std::shared_ptr<Message> DoAckChassisRaw(Message* = nullptr);
            std::shared_ptr<Message> DoAckModeStatus(Message* = nullptr);
            std::shared_ptr<Message> DoAckCoverTrack(Message* = nullptr);
            std::shared_ptr<Message> DoAckLocArray(Message* = nullptr);
            std::shared_ptr<Message> DoAckGridMap(Message* = nullptr);
            std::shared_ptr<Message> DoAckMapStorage(Message* = nullptr);
            std::shared_ptr<Message> DoAckMapID(Message* = nullptr);
            std::shared_ptr<Message> DoAckAppointment(Message* = nullptr);

        private:
            std::mutex mutex_;
            uint32_t us_msg_cnt_ = 0;
            uint64_t us_msg_bytes_ = 0;
            uint32_t ds_msg_cnt_ = 0;
            uint64_t ds_msg_bytes_ = 0;
    };

} //namespace visual
} //namespace camb
} //namespace mstf

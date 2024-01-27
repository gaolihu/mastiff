#pragma once

#include "modules/cambrian/visualize/web_server.h"

namespace mstf {
namespace camb {
namespace visual {

    class VisualClient {
        public:
            VisualClient(const VisualizeOpts*);
            virtual ~VisualClient() final;

            void Init();
            void Start();
            void Stop();
            void Close();

            //send message out to remote side
            int SyncRemoteMap(const std::shared_ptr<OccupancyGrid>&);
            int SyncRemotePath(const std::shared_ptr<MarkerArray>&);
            int SyncRemoteRawImu(const std::shared_ptr<IMUdata>&);
            int SyncRemoteRawOdom(const std::shared_ptr<OdomData>&);
            int SyncRemoteRawLaser(const std::shared_ptr<LaserData>&);
            int SyncRemoteRawMisc(const std::shared_ptr<ChassisMixData>&);
            //void CacheFactory(const std::shared_ptr<ChassisFacotryData>&);
            //void UpdateImageRaw(const std::shared_ptr<CameraCaptureFrame>&);
            //void SyncRemoteObject(const std::shared_ptr<AiOutputResult>&);
            //void SyncRemoteImage(const std::shared_ptr<AiRecognitionImg>&);

            template <typename MessageT>
            int SendUpstreamMesages(const std::shared_ptr<Message>&);

        private:
            //handle remote message
            void OnHandleRemote(const std::shared_ptr<Message>&);
            int DoKeepRemoteAlive();

        private:
            std::unique_ptr<WebServer> web_server_;

            int upload_raw_interval_ms_ = 700; //ms
            std::chrono::steady_clock::time_point time_upload_last_;
    };

    template <typename MessageT>
    int VisualClient::SendUpstreamMesages(const
            std::shared_ptr<Message>& msg) {
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                time_now - time_upload_last_).count();

//#ifdef CAMB_PKG_DBG
#if 0
        AINFO << "handle upstream message: " << GetMessageType<MessageT>();
#endif
        /*
        if (MessageTrait::Instance()->MsgTypeUpSysInfo<MessageT>())
            return UpdateRemoteSystemInfo(msg);

        if (MessageTrait::Instance()->MsgTypeUpPeriphInfo<MessageT>() ||
                MessageTrait::Instance()->MsgTypeIsMix<MessageT>())
            return UpdateRemotePeriphInfo(msg);


        if (MessageTrait::Instance()->MsgTypeUpVolaInfo<MessageT>())
            return UpdateRemoteVolatileInfo(msg);

        if (MessageTrait::Instance()->MsgTypeUpChsRaw<MessageT>()) {
            if (pass < upload_raw_interval_ms_)
                return 0;

            time_upload_last_ = time_now;
            return UpdateRemoteChassisRaw(msg);
        }

        if (MessageTrait::Instance()->MsgTypeIsImu<MessageT>()) {
            //AINFO << "Imu" << msg->DebugString();
            if (pass < upload_raw_interval_ms_)
                return 0;

            time_upload_last_ = time_now;
            return UpdateRemoteChassisRaw(msg);
        }

        if (MessageTrait::Instance()->MsgTypeIsOdome<MessageT>()) {
            //AINFO << "Odome" << msg->DebugString();
            if (pass < upload_raw_interval_ms_)
                return 0;

            time_upload_last_ = time_now;
            return UpdateRemoteChassisRaw(msg);
        }

        if (MessageTrait::Instance()->MsgTypeIsLaser<MessageT>()) {
            //AINFO << "Laser";
            if (pass < upload_raw_interval_ms_)
                return 0;

            time_upload_last_ = time_now;
            return UpdateRemoteChassisRaw(msg);
        }

        if (MessageTrait::Instance()->MsgTypeIsFac<MessageT>()) {
            //AINFO << "Factory";
            if (pass < upload_raw_interval_ms_)
                return 0;

            time_upload_last_ = time_now;
            return UpdateRemoteChassisRaw(msg);
        }

        if (MessageTrait::Instance()->MsgTypeIsRawImg<MessageT>()) {
            //AINFO << "Image";
            if (pass < upload_raw_interval_ms_)
                return 0;

            time_upload_last_ = time_now;
            return UpdateRemoteChassisRaw(msg);
        }

        if (MessageTrait::Instance()->MsgTypeUpModeStatus<MessageT>())
            return UpdateRemoteModeStatus(msg);

        if (MessageTrait::Instance()->MsgTypeUpCoverPose<MessageT>(msg))
            return UpdateRemoteCoverTrack(msg);

        if (MessageTrait::Instance()->MsgTypeUpCoverPose<MessageT>(msg))
            return UpdateRemoteLocArray(msg);

        if (MessageTrait::Instance()->MsgTypeUpGridMap<MessageT>())
            return UpdateRemoteGridMap(msg);

        if (MessageTrait::Instance()->MsgTypeIsMap<MessageT>())
            return UpdateRemoteGridMap(msg);

        if (MessageTrait::Instance()->MsgTypeUpMaps<MessageT>())
            return UpdateRemoteMapStorage(msg);

        if (MessageTrait::Instance()->MsgTypeUniqueId<MessageT>())
            return UpdateRemoteMapID(msg);

        if (MessageTrait::Instance()->MsgTypeUpAppTask<MessageT>())
            //TODO
            return HandleMissionCommand(msg);

        AWARN << "no handle for message: " << GetMessageType<MessageT>();
        */

        return -1;
    }

} //namespace Visual
} //namespace camb
} //namespace mstf

#pragma once

#include <vector>

#include "cyber/common/log.h"
#include "cyber/message/message_traits.h"

#include "modules/cambrian/brain/signals_kick.h"
#include "modules/cambrian/brain/data_feedback.h"

namespace mstf {
namespace camb {
namespace brain {

    class MessagePool :
        public SignalsKick,
        public DataFeedback {

        public:
            MessagePool();
            virtual ~MessagePool();

            // enqueue msg
            template <typename MessageT>
            int EnqueueMessage(const
                    std::shared_ptr<MessageT>&);

            // dequeue msg
            template <typename MessageT>
            std::shared_ptr<Message> DequeueMessage();

            //random events write
            inline void ClearRandomEvents() {
                //rnd_evts_->Clear();
                random_events_trigered_ = false;
            }

            inline void SetRandomTriger() {
                random_events_trigered_ = true;
            }

            inline const bool IsRandomTriger() const {
                return random_events_trigered_;
            }

        private:
            //slam
            int _EnqueueMessage(const std::shared_ptr<OccupancyGrid>&);
            int _EnqueueMessage(const std::shared_ptr<PoseStamped>&);

            //chassis raw
            int _EnqueueMessage(const std::shared_ptr<Imu>&);
            int _EnqueueMessage(const std::shared_ptr<PointCloud>&);
            int _EnqueueMessage(const std::shared_ptr<Odometry>&);
            int _EnqueueMessage(const std::shared_ptr<Image>&);
            int _EnqueueMessage(const std::shared_ptr<PointCloud2>&);

            //chassis misc
            int _EnqueueMessage(const std::shared_ptr<ChassisMiscInfo>&);

        public:
            //lock
            std::mutex mutex_;
    };

    template <typename MessageT>
    int MessagePool::EnqueueMessage(const
            std::shared_ptr<MessageT>& msg) {
#if 1
        AERROR << "in message: \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        auto s = cyber::message::GetMessageName<MessageT>();

        if (s.find("OccupancyGrid") != std::string::npos) {
            return _EnqueueMessage(
                    std::dynamic_pointer_cast<
                    OccupancyGrid>(msg));
        }

        if (s.find("PoseStamped") != std::string::npos) {
            return _EnqueueMessage(
                    std::dynamic_pointer_cast<
                    PoseStamped>(msg));
        }

        if (s.find("Odometry") != std::string::npos) {
            return _EnqueueMessage(
                    std::dynamic_pointer_cast<
                    Odometry>(msg));
        }

        if (s.find("PointCloud") != std::string::npos) {
            return _EnqueueMessage(
                    std::dynamic_pointer_cast<
                    PointCloud>(msg));
        }

        if (s.find("Imu") != std::string::npos) {
            return _EnqueueMessage(
                    std::dynamic_pointer_cast<
                    Imu>(msg));
        }

        if (s.find("Image") != std::string::npos) {
            return _EnqueueMessage(
                    std::dynamic_pointer_cast<
                    Image>(msg));
        }

        if (s.find("PointCloud2") != std::string::npos) {
            return _EnqueueMessage(
                    std::dynamic_pointer_cast<
                    PointCloud2>(msg));
        }

        //chassis infos
        if (s.find("ChassisMiscInfo") != std::string::npos) {
            return _EnqueueMessage(
                    std::dynamic_pointer_cast<
                    ChassisMiscInfo>(msg));
        }

        AERROR << "enqueue unsupported message \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";

        return -1;
    }

    template <typename MessageT>
    std::shared_ptr<Message> MessagePool::DequeueMessage() {
#if 0
        AERROR << "out message: \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        auto s = cyber::message::GetMessageName<MessageT>();

        if (s.find("OccupancyGrid") != std::string::npos) {
            return grid_map_;
        }

        if (s.find("PoseStamped") != std::string::npos) {
            return rt_pose_;
        }

        if (s.find("ChassisRawData") != std::string::npos) {
            return chs_raw_;
        }

        AERROR << "dequeue unsupported message: \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";

        return {};
    }

} //namespace brain
} //namespace camb
} //namespace mstf

#pragma once

#include <vector>

#include "cyber/common/log.h"
#include "cyber/message/message_traits.h"

#include "modules/cambrian/brain/signals_kick.h"
#include "modules/cambrian/brain/data_feedback.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace ventura::common_msgs::nav_msgs;
    using namespace ventura::common_msgs::sensor_msgs;
    using namespace ventura::common_msgs::geometry_msgs;

    using namespace apollo;
    using namespace /*mstf::camb::*/proto;
    using namespace /*mstf::*/chss::proto;
    using namespace google::protobuf;

    using DatabaseNotifyHook = std::function<void()>;
    //using ImplementorNotifyHook = std::function<void()>;

    class MessagePool :
        public SignalsKick,
        public DataFeedback {

        public:
            MessagePool();
            virtual ~MessagePool();

            // enqueue msg
            template <typename MessageT>
            void EnqueueMessage(const
                    std::shared_ptr<MessageT>&);

            // dequeue msg
            template <typename MessageT>
            std::shared_ptr<Message> DequeueMessage();

            //random events write
            inline void ClearRandomEvents() {
                rnd_evts_->Clear();
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
            void _EnqueueMessage(const std::shared_ptr<OccupancyGrid>&);
            void _EnqueueMessage(const std::shared_ptr<PoseStamped>&);

            //chassis raw
            void _EnqueueMessage(const std::shared_ptr<Imu>&);
            void _EnqueueMessage(const std::shared_ptr<PointCloud>&);
            void _EnqueueMessage(const std::shared_ptr<Odometry>&);
            void _EnqueueMessage(const std::shared_ptr<Image>&);
            void _EnqueueMessage(const std::shared_ptr<PointCloud2>&);

            //chassis misc
            void _EnqueueMessage(const std::shared_ptr<ChassisMixData>&);
            void _EnqueueMessage(const std::shared_ptr<HfChassisRaw>&);

        public:
            //lock
            std::mutex mutex_;

            //slam data
            std::shared_ptr<OccupancyGrid> grid_map_ {};
            std::shared_ptr<PoseStamped> rt_pose_ {};
            std::shared_ptr<Path> robot_path_ {};

            //chassis
            std::shared_ptr<ChassisRawData> chs_raw_ {};
            std::shared_ptr<Odometry> odom_data_ {};
            std::shared_ptr<PointCloud> pcl_data_ {};
            std::shared_ptr<Imu> imu_data_ {};
            std::shared_ptr<Image> img_data_ {};
            std::shared_ptr<PointCloud2> llsr_data_ {};

            //chassis parse misc
            std::shared_ptr<ChassisMixData> misc_data_ {};
            std::shared_ptr<HfChassisRaw> hfrq_data_ {};

            //chassis periph
            std::shared_ptr<PeriphInformation> pph_info_ {};
            std::shared_ptr<RandomEvents> rnd_evts_ {};

            //biz & system info
            std::shared_ptr<SystemInformation> sys_info_;
            std::shared_ptr<VolatileInformation> vola_info_ {};
            std::shared_ptr<RobotModeStatus> mode_stat_ {};
            std::shared_ptr<AppointmentTask> app_ {};
            std::shared_ptr<MultiZonesParam> mzps_ {};
            std::shared_ptr<FrgNullMsg> msg_null_ {};

            //notify hook
            DatabaseNotifyHook db_notify_hook_ = nullptr;
            //ImplementorNotifyHook dr_notify_hook_ = nullptr;

            //random events comming
            bool random_events_trigered_ = false;
    };

    template <typename MessageT>
    void MessagePool::EnqueueMessage(const
            std::shared_ptr<MessageT>& msg) {
#if 1
        AERROR << "in message: \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
#endif
        auto s = cyber::message::GetMessageName<MessageT>();

        if (s.find("OccupancyGrid") != std::string::npos) {
            _EnqueueMessage(reinterpret_cast<const
                    std::shared_ptr<OccupancyGrid>&>(msg));
            return;
        }

        if (s.find("PoseStamped") != std::string::npos) {
            _EnqueueMessage(reinterpret_cast<const
                    std::shared_ptr<PoseStamped>&>(msg));
            return;
        }

        if (s.find("Odometry") != std::string::npos) {
            _EnqueueMessage(reinterpret_cast<const
                    std::shared_ptr<Odometry>&>(msg));
            return;
        }

        if (s.find("PointCloud") != std::string::npos) {
            _EnqueueMessage(reinterpret_cast<const
                    std::shared_ptr<PointCloud>&>(msg));
            return;
        }

        if (s.find("Imu") != std::string::npos) {
            _EnqueueMessage(reinterpret_cast<const
                    std::shared_ptr<Imu>&>(msg));
            return;
        }

        if (s.find("Image") != std::string::npos) {
            _EnqueueMessage(reinterpret_cast<const
                    std::shared_ptr<Image>&>(msg));
            return;
        }

        if (s.find("PointCloud2") != std::string::npos) {
            _EnqueueMessage(reinterpret_cast<const
                    std::shared_ptr<PointCloud2>&>(msg));
            return;
        }

        AERROR << "enqueue unsupported message \"" <<
            cyber::message::GetMessageName<MessageT>() << "\"";
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

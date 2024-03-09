#pragma once

#include "cyber/common/log.h"

#include "modules/cambrian/proto/cambrian_data.pb.h"
#include "modules/chassis/proto/chss_io.pb.h"

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

    class DataFeedback {
        public:
            DataFeedback();
            virtual ~DataFeedback();

            std::shared_ptr<ChassisMiscInfo> FeedbkChassisMisc();

        protected:
            //external
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

            std::shared_ptr<ChassisMiscInfo> chss_misc_ {};

            //biz & system info
            std::shared_ptr<SystemInformation> sys_info_;
            std::shared_ptr<VolatileInformation> vola_info_ {};
            std::shared_ptr<RobotModeStatus> mode_stat_ {};
            std::shared_ptr<AppointmentTask> app_ {};
            std::shared_ptr<MultiZonesParam> mzps_ {};
            std::shared_ptr<FrgNullMsg> msg_null_ {};

            //random events comming
            bool random_events_trigered_ = false;
    };

} //namespace brain
} //namespace camb
} //namespace mstf

#include "modules/cambrian/brain/data_feedback.h"

namespace mstf {
namespace camb {
namespace brain {

    DataFeedback::DataFeedback() {
#ifdef CAMB_PKG_DBG
        AINFO << "DataFeedback construct";
#endif
        grid_map_ = std::make_shared<OccupancyGrid>();
        rt_pose_ = std::make_shared<PoseStamped>();
        robot_path_ = std::make_shared<Path>();

        odom_data_ = std::make_shared<Odometry>();
        pcl_data_ = std::make_shared<PointCloud>();
        imu_data_ = std::make_shared<Imu>();
        img_data_ = std::make_shared<Image>();
        llsr_data_ = std::make_shared<PointCloud2>();

        chss_misc_ = std::make_shared<ChassisMiscInfo>();
        msg_null_ = std::make_shared<FrgNullMsg>();
    }

    DataFeedback::~DataFeedback() {
#ifdef CAMB_PKG_DBG
        AINFO << "DataFeedback de-construct";
#endif
    }

    std::shared_ptr<ChassisMiscInfo>
        DataFeedback::FeedbkChassisMisc() {
            return chss_misc_;
        }

} //namespace brain
} //namespace camb
} //namespace mstf

#include "modules/cambrian/brain/message_pool.h"

//#include "opencv2/calib3d.hpp"
//#include "opencv2/opencv.hpp"

//using namespace cv;

namespace mstf {
namespace camb {
namespace brain {

#define PEEP_DATA

    MessagePool::MessagePool() {
#ifdef CAMB_PKG_DBG
        AINFO << "MessagePool construct";
#endif
    }

    MessagePool::~MessagePool() {
#ifdef CAMB_PKG_DBG
        AINFO << "MessagePool de-construct";
        AERROR << "chss misc sp: " <<
            ", local sp: " << chss_misc_ <<
            ", use_count: " << chss_misc_.use_count();
#endif
    }

    int MessagePool::_EnqueueMessage(const
            std::shared_ptr<OccupancyGrid>& msg) {
#ifdef PEEP_DATA
        AINFO << "enqueue grid map \n" <<
            msg->DebugString();
#endif
        std::lock_guard<std::mutex> lg(mutex_);
        grid_map_->CopyFrom(*msg);
        return 0;
    }

    int MessagePool::_EnqueueMessage(const
            std::shared_ptr<PoseStamped>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue pose: "
            << msg->DebugString();
#endif
        *robot_path_->add_poses() = *msg;
        return 0;
    }

    //---------- chassis raw data -------------//
    int MessagePool::_EnqueueMessage(const
            std::shared_ptr<Odometry>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue odom:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_odom();
        *chs_raw_->mutable_d_odom() = *msg;
        return 0;
    }

    int MessagePool::_EnqueueMessage(const
            std::shared_ptr<PointCloud>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue pcl:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_pcl();
        *chs_raw_->mutable_d_pcl() = *msg;
        return 0;
    }

    int MessagePool::_EnqueueMessage(const
            std::shared_ptr<Imu>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue imu:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_imu();
        *chs_raw_->mutable_d_imu() = *msg;
        return 0;
    }

    int MessagePool::_EnqueueMessage(const
            std::shared_ptr<Image>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue image:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_img();
        *chs_raw_->mutable_d_img() = *msg;
        return 0;
    }

    int MessagePool::_EnqueueMessage(const
            std::shared_ptr<PointCloud2>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue linelaser:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_llsr();
        *chs_raw_->mutable_d_llsr() = *msg;
        return 0;
    }

    //chassis raw
    int MessagePool::_EnqueueMessage(const
            std::shared_ptr<ChassisMiscInfo>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue chassis misc\n" << msg->DebugString();
#endif
        *chss_misc_ = *msg;
        return KickImplementorHook();
    }

    //---------- chassis raw data -------------//

} //namespace brain
} //namespace camb
} //namespace mstf

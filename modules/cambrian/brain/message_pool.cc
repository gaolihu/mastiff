#include "modules/cambrian/brain/message_pool.h"

//#include "opencv2/calib3d.hpp"
//#include "opencv2/opencv.hpp"

//using namespace cv;

namespace mstf {
namespace camb {
namespace brain {

    MessagePool::MessagePool() {
#ifdef CAMB_PKG_DBG
        AINFO << "MessagePool construct";
#endif
        grid_map_ = std::make_shared<OccupancyGrid>();
        rt_pose_ = std::make_shared<PoseStamped>();
        robot_path_ = std::make_shared<Path>();

        odom_data_ = std::make_shared<Odometry>();
        pcl_data_ = std::make_shared<PointCloud>();
        imu_data_ = std::make_shared<Imu>();
        img_data_ = std::make_shared<Image>();
        llsr_data_ = std::make_shared<PointCloud2>();

        //TODO
        pph_info_ = std::make_shared<PeriphInformation>();
        rnd_evts_ = std::make_shared<RandomEvents>();
        msg_null_ = std::make_shared<FrgNullMsg>();
    }

    MessagePool::~MessagePool() {
#ifdef CAMB_PKG_DBG
        AINFO << "MessagePool de-construct";
        AERROR << "volatile sp: " <<
            ", local sp: " << vola_info_ <<
            ", use_count: " << vola_info_.use_count();
#endif
    }

    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<OccupancyGrid>& msg) {
#ifdef PEEP_DATA
        AINFO << "enqueue grid map \n" <<
            msg->DebugString();
#endif
        std::lock_guard<std::mutex> lg(mutex_);
        grid_map_->CopyFrom(*msg);
    }

    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<PoseStamped>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue pose: "
            << msg->DebugString();
#endif
        *robot_path_->add_poses() = *msg;
    }

    //---------- chassis raw data -------------//
    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<Odometry>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue odom:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_odom();
        *chs_raw_->mutable_d_odom() = *msg;
    }

    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<PointCloud>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue pcl:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_pcl();
        *chs_raw_->mutable_d_pcl() = *msg;
    }

    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<Imu>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue imu:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_imu();
        *chs_raw_->mutable_d_imu() = *msg;
    }

    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<Image>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue image:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_img();
        *chs_raw_->mutable_d_img() = *msg;
    }

    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<PointCloud2>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);
#ifdef PEEP_DATA
        AINFO << "enqueue linelaser:\n" <<
            msg->DebugString();
#endif
        //chs_raw_->release_d_llsr();
        *chs_raw_->mutable_d_llsr() = *msg;
    }

    //mix data
    // TODO
    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<ChassisMixData>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);

        ClearRandomEvents();

#ifdef PEEP_DATA
        AINFO << "enqueue chassis misc:\n" <<
            msg->DebugString();
#endif
        if (!msg->has_pp_info()) {
            AWARN << "no ppi:\n" << msg->DebugString();
        }

        *misc_data_ = *msg;
    }

#if 0
        if (msg->pp_info().pp_gpio().has_toggle_button())
            SetToggleButton(msg->pp_info().pp_gpio().
                    toggle_button().value());

        if (msg->pp_info().pp_gpio().has_key_power())
            SetShortKeyPower(msg->pp_info().pp_gpio().
                    key_power().value());

        if (msg->pp_info().pp_gpio().has_key_home())
            SetShortKeyHome(msg->pp_info().pp_gpio().
                    key_home().value());

        if (msg->pp_info().pp_gpio().has_key_power_l())
            SetLongKeyPower(msg->pp_info().pp_gpio().
                    key_power_l().value());

        if (msg->pp_info().pp_gpio().has_key_home_l())
            SetLongKeyHome(msg->pp_info().pp_gpio().
                    key_home_l().value());

        if (msg->pp_info().pp_gpio().has_no_water())
            SetNoWater(msg->pp_info().pp_gpio().
                    no_water().value());

        if (msg->pp_info().pp_gpio().has_water_full())
            SetFullWater(msg->pp_info().pp_gpio().
                    water_full().value());

        if (msg->pp_info().pp_gpio().has_dust_box())
            SetDustBox(msg->pp_info().pp_gpio().
                    dust_box().value());

        if (msg->pp_info().pp_gpio().has_water_tank())
            SetWaterTank(msg->pp_info().pp_gpio().
                    water_tank().value());

        if (msg->pp_info().pp_gpio().has_left_pallet())
            SetLeftPallet(msg->pp_info().pp_gpio().
                    left_pallet().value());

        if (msg->pp_info().pp_gpio().has_right_pallet())
            SetRightPallet(msg->pp_info().pp_gpio().
                    right_pallet().value());

        if (msg->pp_info().pp_gpio().has_bump_left())
            SetLeftBump(msg->pp_info().pp_gpio().
                    bump_left().value());

        if (msg->pp_info().pp_gpio().has_bump_right())
            SetRightBump(msg->pp_info().pp_gpio().
                    bump_right().value());

        if (msg->pp_info().pp_gpio().has_lbump_left())
            SetLidarLeftBump(msg->pp_info().pp_gpio().
                    lbump_left().value());

        if (msg->pp_info().pp_gpio().has_lbump_right())
            SetLidarRightBump(msg->pp_info().pp_gpio().
                    lbump_right().value());

        if (msg->pp_info().pp_gpio().has_lbump_front())
            SetLidarFrontBump(msg->pp_info().pp_gpio().
                    lbump_front().value());

        if (msg->pp_info().pp_gpio().has_lbump_top())
            SetLidarTopBump(msg->pp_info().pp_gpio().
                    lbump_top().value());

        if (msg->pp_info().pp_gpio().has_wheel_lift_left())
            SetWheelLeftLift(msg->pp_info().pp_gpio().
                    wheel_lift_left().value());

        if (msg->pp_info().pp_gpio().has_wheel_lift_right())
            SetWheelRightLift(msg->pp_info().pp_gpio().
                    wheel_lift_right().value());

        if (msg->pp_info().pp_gpio().has_along_wall_trg())
            SetAlongWallTrig(msg->pp_info().pp_gpio().
                    along_wall_trg().value());

        if (msg->pp_info().pp_gpio().has_lwhl_pulse())
            SetWheelLeftPulse(msg->pp_info().pp_gpio().
                    lwhl_pulse().value());

        if (msg->pp_info().pp_gpio().has_rwhl_pulse())
            SetWheelRightPulse(msg->pp_info().pp_gpio().
                    rwhl_pulse().value());

        if (msg->pp_info().pp_gpio().has_lwhl_speed())
            SetWheelLeftSpeed(msg->pp_info().pp_gpio().
                    lwhl_speed().value());

        if (msg->pp_info().pp_gpio().has_rwhl_speed())
            SetWheelRightSpeed(msg->pp_info().pp_gpio().
                    rwhl_speed().value());

        if (msg->pp_info().pp_adc().has_ground_signal_left())
            SetDropLeft(msg->pp_info().pp_adc().
                    ground_signal_left().value());

        if (msg->pp_info().pp_adc().has_ground_signal_mleft())
            SetDropMidLeft(msg->pp_info().pp_adc().
                    ground_signal_mleft().value());

        if (msg->pp_info().pp_adc().has_ground_signal_mright())
            SetDropMidRight(msg->pp_info().pp_adc().
                    ground_signal_mright().value());

        if (msg->pp_info().pp_adc().has_ground_signal_right())
            SetDropRight(msg->pp_info().pp_adc().
                    ground_signal_right().value());

        if (msg->pp_info().pp_adc().has_ground_signal_left_rear())
            SetDropLeftRear(msg->pp_info().pp_adc().
                    ground_signal_left_rear().value());

        if (msg->pp_info().pp_adc().has_ground_signal_right_rear())
            SetDropRightRear(msg->pp_info().pp_adc().
                    ground_signal_right_rear().value());

        if (msg->pp_info().pp_infra().has_ground_l())
            SetDropLeftAdc(msg->pp_info(). pp_infra().
                    ground_l().value());

        if (msg->pp_info().pp_infra().has_ground_l_front())
            SetDropMidLeftAdc(msg->pp_info().pp_infra().
                    ground_l_front().value());

        if (msg->pp_info().pp_infra().has_ground_r_front())
            SetDropMidRightAdc(msg->pp_info().pp_infra().
                    ground_r_front().value());

        if (msg->pp_info().pp_infra().has_ground_r())
            SetDropRightAdc(msg->pp_info().pp_infra().
                    ground_r().value());

        if (msg->pp_info().pp_infra().has_ground_l_rear())
            SetDropLeftRearAdc(msg->pp_info().pp_infra().
                    ground_l_rear().value());

        if (msg->pp_info().pp_infra().has_ground_r_rear())
            SetDropRightRearAdc(msg->pp_info().pp_infra().
                    ground_r_rear().value());

        if (msg->pp_info().pp_adc().has_charge_touch_dock())
            SetChargeTouchDock(msg->pp_info().pp_adc().
                    charge_touch_dock().value());

        if (msg->pp_info().pp_adc().has_charge_status_charging())
            SetChargeOnDock(msg->pp_info().pp_adc().
                    charge_status_charging().value());

        if (msg->pp_info().pp_adc().has_charge_status_finish())
            SetChargeFinished(msg->pp_info().pp_adc().
                    charge_status_finish().value());

        if (msg->pp_info().pp_adc().has_middle_scan_current())
            SetMidScanDetect(msg->pp_info().pp_adc().
                    middle_scan_current().value());

        if (msg->pp_info().pp_adc().has_low_power_charge())
            SetLowPowerDocking(msg->pp_info().pp_adc().
                    low_power_charge().value());

        if (msg->pp_info().pp_adc().has_low_power_off())
            SetLowPowerPoweroff(msg->pp_info().pp_adc().
                    low_power_off().value());

        if (msg->pp_info().pp_adc().has_leftw_current_ma())
            SetWheelLeftCurrent(msg->pp_info().pp_adc().
                    leftw_current_ma().value());

        if (msg->pp_info().pp_adc().has_rightw_current_ma())
            SetWheelRightCurrent(msg->pp_info().pp_adc().
                    rightw_current_ma().value());

        if (msg->pp_info().pp_infra().has_tof_distance())
            SetTofDistance(msg->pp_info().pp_infra().
                    tof_distance().value());

        //dock signal
        if (msg->pp_info().pp_infra().has_dock_l())
            SetDockLeftSignal(msg->pp_info().pp_infra().
                    dock_l().value());

        if (msg->pp_info().pp_infra().has_dock_ml())
            SetDockMidLeftSignal(msg->pp_info().pp_infra().
                    dock_ml().value());

        if (msg->pp_info().pp_infra().has_dock_mr())
            SetDockMidRightSignal(msg->pp_info().pp_infra().
                    dock_mr().value());

        if (msg->pp_info().pp_infra().has_dock_front_r())
            SetDockRightFrontSignal(msg->pp_info().pp_infra().
                    dock_front_r().value());

        if (msg->pp_info().pp_infra().has_dock_r())
            SetDockRightSignal(msg->pp_info().pp_infra().
                    dock_r().value());

        if (msg->pp_info().pp_infra().has_dock_front_l())
            SetDockLeftFrontSignal(msg->pp_info().pp_infra().
                    dock_front_l().value());

        if (msg->pp_info().pp_infra().has_dock_rear_ml())
            SetDockRearMidLeftSignal(msg->pp_info().pp_infra().
                    dock_rear_ml().value());

        if (msg->pp_info().pp_infra().has_dock_rear_mr())
            SetDockRearMidRightSignal(msg->pp_info().pp_infra().
                    dock_rear_mr().value());

        if (msg->pp_info().pp_extra().has_battery())
            SetBatteryLevel(msg->pp_info().pp_extra().
                    battery().value());

        if (msg->pp_info().pp_extra().has_bat_temp())
            SetBatteryTemp(msg->pp_info().pp_extra().
                    bat_temp().value());

        if (msg->pp_info().pp_extra().has_online_result())
            SetOnlineResult(msg->pp_info().pp_extra().
                    online_result().value());

        if (msg->pp_info().pp_extra().has_proto_ver())
            SetOnlineVersion(msg->pp_info().pp_extra().
                    proto_ver().value());

        if (msg->pp_info().pp_extra().has_heart_beat_result())
            SetOnlineHeartBeat(msg->pp_info().pp_extra().
                    heart_beat_result().value());

        if (msg->pp_info().pp_extra().has_mcu_sn())
            SetMcuSn(msg->pp_info().pp_extra().
                    mcu_sn().value());

        if (msg->pp_info().pp_extra().has_mcu_ver())
            SetMcuVersion(msg->pp_info().pp_extra().
                    mcu_ver().value());

        if (msg->pp_info().pp_extra().has_mcu_comp_time())
            SetMcuCompileTime(msg->pp_info().pp_extra().
                    mcu_comp_time().value());

        if (msg->pp_info().pp_extra().has_sensor_en_stat())
            SetMcuSensorEnStat(msg->pp_info().pp_extra().
                    sensor_en_stat().wall_en().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().drop_en().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().tof_en().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().wheels_power().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().fan_power().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().main_brush_power().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().side_brush_power().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().pump_motor_power().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().charge_en().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().robot_power().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().pump_en().value(),
                    msg->pp_info().pp_extra().
                    sensor_en_stat().pallet_en().value());

        if (msg->pp_info().pp_extra().has_ctrl_chass_result()) {
            if (msg->pp_info().pp_extra().ctrl_chass_result().
                    has_ctrl_result() &&
                    msg->pp_info().pp_extra().ctrl_chass_result().
                    has_ctrl_ack_type()) {
                SetCtrlSlaveResult(msg->pp_info().pp_extra().
                        ctrl_chass_result().ctrl_result().value(),
                        msg->pp_info().pp_extra().
                        ctrl_chass_result().ctrl_ack_type().value());
            }
        }

        if (msg->pp_info().pp_extra().has_camera_flip())
            SetFlipCameraStatus(msg->pp_info().pp_extra().
                    camera_flip().value());

        if (msg->pp_info().pp_extra().has_error_code())
            SetSlaveErrorCode(msg->pp_info().pp_extra().
                    error_code().value());

        if (msg->pp_info().pp_extra().has_slave_10th1_ms()) {
            SetSlaveSysTime(msg->pp_info().pp_extra().
                    slave_10th1_ms().value());
        }

        if (IsRandomTriger()) {
            PublishRandomEvents();
        }
#endif

    void MessagePool::_EnqueueMessage(const
            std::shared_ptr<HfChassisRaw>& msg) {
        std::lock_guard<std::mutex> lg(mutex_);

#ifdef PEEP_DATA
        AINFO << "enqueue hcr\n" << msg->DebugString();
#endif
    }

#if 0
        if (pph_info_.get() == nullptr)
            pph_info_ = std::make_shared<PeriphInformation>();

        //1, tof
        if (msg->has_tof_distance()) {
            SetTofDistance(msg->tof_distance().value());
        }

        //2, wheel current
        if (msg->has_leftw_current_ma()) {
            SetWheelLeftCurrent(msg->leftw_current_ma().value());
        }

        if (msg->has_rightw_current_ma()) {
            SetWheelRightCurrent(msg->rightw_current_ma().value());
        }

        //3, dock signal
        if (msg->has_dock_l()) {
            SetDockLeftSignal(msg->dock_l().value());
        }

        if (msg->has_dock_ml()) {
            SetDockMidLeftSignal(msg->dock_ml().value());
        }

        if (msg->has_dock_mr()) {
            SetDockMidRightSignal(msg->dock_mr().value());
        }

        if (msg->has_dock_front_r()) {
            SetDockRightFrontSignal(msg->dock_front_r().value());
        }

        if (msg->has_dock_r()) {
            SetDockRightSignal(msg->dock_r().value());
        }

        if (msg->has_dock_front_l()) {
            SetDockLeftFrontSignal(msg->dock_front_l().value());
        }

        if (msg->has_dock_rear_ml()) {
            SetDockRearMidLeftSignal(msg->dock_rear_ml().value());
        }

        if (msg->has_dock_rear_mr()) {
            SetDockRearMidRightSignal(msg->dock_rear_mr().value());
        }

        //6, wheel pulse
        if (msg->has_lwhl_pulse()) {
            SetWheelLeftPulse(msg->lwhl_pulse().value());
        }

        if (msg->has_rwhl_pulse()) {
            SetWheelRightPulse(msg->rwhl_pulse().value());
        }

        //8, slave sys time
        if (msg->has_slave_10th1_ms()) {
            SetSlaveSysTime(msg->slave_10th1_ms().value());
        }

        //9, drop adc
        if (msg->has_ground_l()) {
            SetDropLeftAdc(msg->ground_l().value());
        }

        if (msg->has_ground_l_front()) {
            SetDropMidLeftAdc(msg->ground_l_front().value());
        }

        if (msg->has_ground_r_front()) {
            SetDropMidRightAdc(msg->ground_r_front().value());
        }

        if (msg->has_ground_r()) {
            SetDropRightAdc(msg->ground_r().value());
        }

        if (msg->has_ground_l_rear()) {
            SetDropLeftRearAdc(msg->ground_l_rear().value());
        }

        if (msg->has_ground_r_rear()) {
            SetDropRightRearAdc(msg->ground_r_rear().value());
        }

        //10, wheel speed
        if (msg->has_lwhl_speed()) {
            SetWheelLeftSpeed(msg->lwhl_speed().value());
        }

        if (msg->has_rwhl_speed()) {
            SetWheelRightSpeed(msg->rwhl_speed().value());
        }
#endif

    //---------- chassis raw data -------------//

} //namespace brain
} //namespace camb
} //namespace mstf

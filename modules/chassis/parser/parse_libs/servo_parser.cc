#include "cyber/common/log.h"

#include "Eigen/Core"
#include "Eigen/Dense"

#include "modules/chassis/parser/parse_libs/servo_parser.h"
#include "modules/chassis/parser/parse_libs/ds20270da_driver.h"

namespace mstf {
namespace chss {
namespace parser {

    ServoParser::ServoParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "ServoParser construct";
        }

    ServoParser::~ServoParser() {
        AINFO << "ServoParser de-construct";
    }

    int ServoParser::Init() {
        AINFO << "wheel diameter: " <<
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().wheel_diameter_10th1_mm() <<
            " 1/10mm | distance: " <<
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().wheel_distance_10th1_mm() <<
            " 1/10mm, acc: " <<
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().accelerate_time_ms() <<
            " | dec time: " <<
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().decelerate_time_ms();
        AINFO <<"reporting speed period: " <<
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().speed_report_period_ms() <<
            "ms | status: " <<
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().status_report_period_ms() <<
            ", resolution: " <<
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().servo_motor_resolution() <<
            ", reduction ratio: " <<
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().servo_motor_reduction();
        config_servo_motor_parameters(
                dynamic_cast<const ServoDevice&>(GetDevice()).
                servo_info().wheel_diameter_10th1_mm(),
                dynamic_cast<const ServoDevice&>(GetDevice()).
                servo_info().wheel_distance_10th1_mm(),
                dynamic_cast<const ServoDevice&>(GetDevice()).
                servo_info().accelerate_time_ms(),
                dynamic_cast<const ServoDevice&>(GetDevice()).
                servo_info().decelerate_time_ms(),
                dynamic_cast<const ServoDevice&>(GetDevice()).
                servo_info().speed_report_period_ms(),
                dynamic_cast<const ServoDevice&>(GetDevice()).
                servo_info().status_report_period_ms());
        return ParserBaseItf::Init(
                dynamic_cast<const ServoDevice&>
                (GetDevice()).sn_ind().port(),
                &dynamic_cast<const ServoDevice&>(GetDevice()).
                can_conf().buf_setting());
    }

    int ServoParser::WriteServoMessage(const DownToServoData& data) {
#if 0
        AINFO << "data to servo motor:\n" << data.DebugString();
#endif
#if 0
        //for testing
        std::vector<uint8_t> packed_data =
            packer_->PackMotorMessageString(data);
        return RawManage::Instance()->WriteCan(packed_data);
#else
        if (data.has_motor_speed()) {
            //set motor speed
            if (data.motor_speed().use_diff_speed()) {
#ifdef USE_FREE_CAN_PROTOCOL // 0
                //use diff speed
                std::tuple<const int,
                    const std::vector<uint8_t>> packed_data =
                        packer_->PackMotorDiffSpeed(data);
                return RawManage::Instance()->
                    WriteCan(std::get<0>(packed_data),
                            &(std::get<1>(packed_data))[0],
                            std::get<1>(packed_data).size());
#else
                std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>> packed_datas =
                        packer_->PackMotorDiffSpeedDouble(data);
                for (int i = 0; i < (int)packed_datas.size(); i++) {
                    RawManage::Instance()->
                        WriteCan(std::get<0>(packed_datas[i]),
                                &(std::get<1>(packed_datas[i]))[0],
                                std::get<1>(packed_datas[i]).size());
                    //??? delay for two wheels ???
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
#endif
            } else {
                //use wheel speed
                std::tuple<const int,
                    const std::vector<uint8_t>> packed_data =
                        packer_->PackMotorWheelSpeed(data);
                return RawManage::Instance()->
                    WriteCan(std::get<0>(packed_data),
                            &(std::get<1>(packed_data))[0],
                            std::get<1>(packed_data).size());
            }
        } else {
            //config motor
            std::vector<std::tuple<const int,
                const std::vector<uint8_t>>> packed_datas =
                    packer_->PackMotorMessageArrayRaw(data);
            AINFO << "send commands num: " << packed_datas.size();
            for (int i = 0; i < (int)packed_datas.size(); i++) {
                RawManage::Instance()->
                    WriteCan(std::get<0>(packed_datas[i]),
                            &(std::get<1>(packed_datas[i]))[0],
                            std::get<1>(packed_datas[i]).size());
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
#endif
        return 0;
    }

//#define SERVO_DBG 1
    int ServoParser::ParseCanBuffer(const uint8_t* buf,
            const size_t len) {
        //if the current frame is good enough to parse
        //a whole frame of data, send upstream directly

        int encoder_left = -1;
        int encoder_right = -1;
        int rpm_left = -1;
        int rpm_right = -1;
        int status_left = -1;
        int status_right = -1;

        parse_servo_motor_info(buf, len,
                &rpm_left, &rpm_right,
                &encoder_left, &encoder_right,
                &status_left, &status_right);
        if (rpm_left != -1 || encoder_left != -1) {
            left_rpm_ = rpm_left;
            left_encoder_ = encoder_left;
#ifdef SERVO_DBG
            AINFO << "RPM L: " << left_rpm_ <<
                ", Encoder L: " << left_encoder_ <<
                "    length " << len <<
                ", [" <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[0]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[1]) <<
                " | " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[2]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[3]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[4]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[5]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[6]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[7]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[8]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[9]) <<
                " ]";
#endif
        }
        if (rpm_right != -1 || encoder_right != -1) {
            right_rpm_ = -rpm_right;
            right_encoder_ = -encoder_right;

#ifdef SERVO_DBG
            AINFO << "RPM R: " << right_rpm_ <<
                ", Encoder R: " << right_encoder_ <<
                "    length " << len <<
                ", [" <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[0]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[1]) <<
                " | " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[2]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[3]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[4]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[5]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[6]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[7]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[8]) <<
                " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[9]) <<
                " ]";
#endif
            ParseSigleFrame({}, 0);

            last_left_encoder_ = left_encoder_;
            last_right_encoder_ = right_encoder_;
            last_left_rpm_ = left_rpm_;
            last_right_rpm_ = right_rpm_;
        }
        if (status_left != -1) {
            if (status_left != 0x37) {
                AWARN << "status L: " << status_left <<
                    "    len " << len <<
                    ", [" <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[0]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[1]) <<
                    " | " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[2]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[3]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[4]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[5]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[6]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[7]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[8]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[9]) <<
                    " ]";
                left_status_ = status_left;
            }
        }
        if (status_right != -1) {
            if (status_right != 0x37) {
                AWARN << "status R: " << status_right <<
                    "    len " << len <<
                    ", [" <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[0]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[1]) <<
                    " | " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[2]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[3]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[4]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[5]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[6]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[7]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[8]) <<
                    " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[9]) <<
                    " ]";
                right_status_ = status_right;
            }
        }

        return 0;
    }

    int ServoParser::ParseSigleFrame(const
            std::vector<uint8_t>& data,
            const size_t len) {
        ventura::common_msgs::nav_msgs::Odometry odom;

        odom.mutable_header()->set_seq(odom_seq_++);
#if 1
        timeval tv;
        gettimeofday(&tv, NULL);

        odom.mutable_header()->mutable_stamp()->set_sec(tv.tv_sec);
        odom.mutable_header()->mutable_stamp()->set_nsec(tv.tv_usec * 1000);
        odom.mutable_header()->set_frame_id("odom");
#else
        auto tm = cyber::Time::Now();

        odom.mutable_header()->set_seq(odom_seq_++);
        odom.mutable_header()->mutable_stamp()->set_sec(tm.ToSecond());
        odom.mutable_header()->mutable_stamp()->set_nsec(tm.ToNanosecond());
#endif
        odom.set_child_frame_id("base_link");

        float d_left = M_PI * dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().wheel_diameter_10th1_mm() *
            (left_encoder_ - last_left_encoder_ /*+ left_scale * encoder_scale_val*/) /
            ((dynamic_cast<const ServoDevice&>(GetDevice()).
              servo_info().servo_motor_resolution() /**
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().servo_motor_reduction()*/) * 1e4);

        float d_right = M_PI * dynamic_cast<const ServoDevice&>(GetDevice()).
        servo_info().wheel_diameter_10th1_mm() *
            (right_encoder_ - last_right_encoder_ /*+ right_scale * encoder_scale_val*/) /
            ((dynamic_cast<const ServoDevice&>(GetDevice()).
              servo_info().servo_motor_resolution() /**
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().servo_motor_reduction()*/) * 1e4);

        float ds = (d_left + d_right) / 2;
        float dth = (d_right - d_left) * 1e4 /
            dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().wheel_distance_10th1_mm();

        pose_x_ += ds * (cos(pose_theta_ + dth / 2.f));
        pose_y_ += ds * (sin(pose_theta_ + dth / 2.f));
        pose_theta_ += dth;

        /*
        auto q = Eigen::AngleAxisd(pose_theta_, Eigen::Vector3d::UnitZ()) *
            Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX()) *
            Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY());
            */
        auto q = Eigen::Quaterniond(Eigen::AngleAxisd(pose_theta_, Eigen::Vector3d::UnitZ()));

        odom.mutable_pose()->mutable_pose()->mutable_position()->set_x(pose_x_);
        odom.mutable_pose()->mutable_pose()->mutable_position()->set_y(pose_y_);
        //odom.mutable_pose()->mutable_pose()->mutable_position()->set_z(0);
        for (int i = 0; i < 36; i++)
            odom.mutable_pose()->add_covariance(0.f);

        odom.mutable_pose()->mutable_pose()->mutable_orientation()->set_x(q.x());
        odom.mutable_pose()->mutable_pose()->mutable_orientation()->set_y(q.y());
        odom.mutable_pose()->mutable_pose()->mutable_orientation()->set_z(q.z());
        odom.mutable_pose()->mutable_pose()->mutable_orientation()->set_w(q.w());

        float vl = left_rpm_ * M_PI * dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().wheel_diameter_10th1_mm() / 6e6;
        float vr = right_rpm_ * M_PI * dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().wheel_diameter_10th1_mm() / 6e6;

        float line = (vl + vr) / 2;
        float omg = (vr - vl) * 1e4 / dynamic_cast<const ServoDevice&>(GetDevice()).
            servo_info().wheel_distance_10th1_mm();

        //speed
        odom.mutable_twist()->mutable_twist()->mutable_linear()->set_x(line);
        //odom.mutable_twist()->mutable_twist()->mutable_linear()->set_y();
        //odom.mutable_twist()->mutable_twist()->mutable_linear()->set_z();
        //odom.mutable_twist()->mutable_twist()->mutable_angular()->set_x();
        //odom.mutable_twist()->mutable_twist()->mutable_angular()->set_y();
        odom.mutable_twist()->mutable_twist()->mutable_angular()->set_z(omg);
        for (int i = 0; i < 36; i++)
            odom.mutable_twist()->add_covariance(0.f);

        //AINFO_EVERY(100) << "odom:\n" << odom.DebugString();

//#define SERVO_DBG
#ifdef SERVO_DBG
        AINFO << "L: [" << left_encoder_ <<
            ", " << last_left_encoder_ <<
            "], R: [" << right_encoder_ <<
            ", " << last_right_encoder_ <<
            "], dL: " << d_left <<
            ", dR: " << d_right <<
            ", O: " << ds <<
            ", dth: " << dth <<
            ", pose x: " << pose_x_ <<
            ", y: " << pose_y_ <<
            ", theta: " << pose_theta_;
#endif
#if 0
//#ifdef SERVO_DBG
        AINFO << "velocity L/R: [ " << vl << "m/s" <<
            ", " << vr << " ]m/s" <<
            ", line speed: " << line << "m/s" <<
            ", omg: " << omg << "rad/s";
#endif
        //TF
        /*
        apollo::transform::TransformStamped tf2_msg;

        auto mutable_head = tf2_msg.mutable_header();
        mutable_head->set_timestamp_sec(tv.tv_sec + tv.tv_usec / 1e3);
        mutable_head->set_frame_id("odom");

        tf2_msg.set_child_frame_id("base_link");

        auto mutable_translation = tf2_msg.mutable_transform()->mutable_translation();
        mutable_translation->set_x(pose_x_);
        mutable_translation->set_y(pose_y_);

        //mutable_translation->set_z(0);
        auto mutable_rotation = tf2_msg.mutable_transform()->mutable_rotation();
        mutable_rotation->set_qx(q.x());
        mutable_rotation->set_qy(q.y());
        mutable_rotation->set_qz(q.z());
        mutable_rotation->set_qw(q.w());

        tf_broadcaster_->SendTransform(tf2_msg);

        tf_broadcaster_.reset(new apollo::transform::TransformBroadcaster(node_cyber_));
        */

        return frame_processor_(&odom, "ventura::common_msgs::nav_msgs::Odometry");
    }

} //namespace parser
} //namespace chss
} //namespace mstf

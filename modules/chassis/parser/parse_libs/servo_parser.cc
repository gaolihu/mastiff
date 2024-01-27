#include "Eigen/Core"
#include "Eigen/Dense"

#include "modules/chassis/parser/parse_drv_link.h"
#include "modules/chassis/parser/parse_libs/ds20270da_driver.h"
#include "modules/chassis/parser/parse_libs/servo_parser.h"

namespace mstf {
namespace chss {
namespace parser {

    ServoParser::ServoParser(
            const SensorIndicator& si) :
        ParserBaseItf(si) {
#ifdef CHSS_PKG_DBG
            AINFO << "ServoParser construct";
#endif
            ParseDrvLink::Instance()->RegisterCanListener
                (std::bind(&ParserBaseItf::OnOriginalDataCan,
                           this, ::_1, ::_2, ::_3), s_idc_);
        }

    ServoParser::~ServoParser() {
#ifdef CHSS_PKG_DBG
        AINFO << "ServoParser de-construct";
#endif
    }

    int ServoParser::Init() {
        AINFO << "wheel diameter: " <<
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().wheel_diameter_10th1_mm() <<
            " 1/10mm | distance: " <<
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().wheel_distance_10th1_mm() <<
            " 1/10mm, acc: " <<
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().accelerate_time_ms() <<
            " | dec time: " <<
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().decelerate_time_ms();
        AINFO <<"reporting speed period: " <<
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().speed_report_period_ms() <<
            "ms | status: " <<
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().status_report_period_ms() <<
            ", resolution: " <<
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().servo_motor_resolution() <<
            ", reduction ratio: " <<
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().servo_motor_reduction();
        config_servo_motor_parameters(
                dynamic_cast<const ServoDevConf*>(GetDevConfig())->
                servo_info().wheel_diameter_10th1_mm(),
                dynamic_cast<const ServoDevConf*>(GetDevConfig())->
                servo_info().wheel_distance_10th1_mm(),
                dynamic_cast<const ServoDevConf*>(GetDevConfig())->
                servo_info().accelerate_time_ms(),
                dynamic_cast<const ServoDevConf*>(GetDevConfig())->
                servo_info().decelerate_time_ms(),
                dynamic_cast<const ServoDevConf*>(GetDevConfig())->
                servo_info().speed_report_period_ms(),
                dynamic_cast<const ServoDevConf*>(GetDevConfig())->
                servo_info().status_report_period_ms());

        ParseDrvLink::Instance()->Init(s_idc_);

        return ParserBaseItf::Init();
    }

    int ServoParser::Start() {
        if (ParserBaseItf::Start() != 0) {
            AERROR << "ServoParser start error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Start(s_idc_);
    }

    int ServoParser::Stop() {
        if (ParserBaseItf::Stop() != 0) {
            AERROR << "ServoParser stop error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Stop(s_idc_);
    }

    int ServoParser::Resume() {
        if (ParserBaseItf::Resume() != 0) {
            AERROR << "ServoParser resume error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Resume(s_idc_);
    }

    int ServoParser::Close() {
        if (ParserBaseItf::Close() != 0) {
            AERROR << "ServoParser close error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Close(s_idc_);
    }

    int ServoParser::WriteServoMessage(const
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& data) {
        for (int i = 0; i < (int)data.size(); i++) {
#if 0
            AINFO << "write servo messages " << i;
#endif
            if (ParseDrvLink::Instance()->
                WriteCan(s_idc_, std::get<0>(data[i]),
                        &(std::get<1>(data[i]))[0],
                        std::get<1>(data[i]).size()) !=
                std::get<1>(data[i]).size()) {
                AERROR << "write servo error!";
            }

            if (i % 5 == 0) {
                //for multi pack of data sending
                std::this_thread::sleep_for(std::
                        chrono::milliseconds(1));
            }
        }

        return 0;
    }

//#define SERVO_DBG 1
    int ServoParser::ParseCanBuffer(const
            SensorIndicator* si,
            const uint8_t* buf,
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
            std::vector<uint8_t>& data, const size_t len) {
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

        float d_left = M_PI * dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().wheel_diameter_10th1_mm() *
            (left_encoder_ - last_left_encoder_ /*+ left_scale * encoder_scale_val*/) /
            ((dynamic_cast<const ServoDevConf*>(GetDevConfig())->
              servo_info().servo_motor_resolution() /**
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().servo_motor_reduction()*/) * 1e4);

        float d_right = M_PI * dynamic_cast<const ServoDevConf*>(GetDevConfig())->
        servo_info().wheel_diameter_10th1_mm() *
            (right_encoder_ - last_right_encoder_ /*+ right_scale * encoder_scale_val*/) /
            ((dynamic_cast<const ServoDevConf*>(GetDevConfig())->
              servo_info().servo_motor_resolution() /**
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().servo_motor_reduction()*/) * 1e4);

        float ds = (d_left + d_right) / 2;
        float dth = (d_right - d_left) * 1e4 /
            dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().wheel_distance_10th1_mm();

        pose_x_ += ds * (cos(pose_theta_ + dth / 2.f));
        pose_y_ += ds * (sin(pose_theta_ + dth / 2.f));
        pose_theta_ += dth;

        auto q = Eigen::Quaterniond(Eigen::AngleAxisd(pose_theta_, Eigen::Vector3d::UnitZ()));

        odom.mutable_pose()->mutable_pose()->mutable_position()->set_x(pose_x_);
        odom.mutable_pose()->mutable_pose()->mutable_position()->set_y(pose_y_);
        //odom.mutable_pose()->mutable_pose()->mutable_position()->set_z(0);
        odom.mutable_pose()->mutable_covariance()->Resize(36, 0.f);

        odom.mutable_pose()->mutable_pose()->mutable_orientation()->set_x(q.x());
        odom.mutable_pose()->mutable_pose()->mutable_orientation()->set_y(q.y());
        odom.mutable_pose()->mutable_pose()->mutable_orientation()->set_z(q.z());
        odom.mutable_pose()->mutable_pose()->mutable_orientation()->set_w(q.w());

        float vl = left_rpm_ * M_PI * dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().wheel_diameter_10th1_mm() / 6e6;
        float vr = right_rpm_ * M_PI * dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().wheel_diameter_10th1_mm() / 6e6;

        float line = (vl + vr) / 2;
        float omg = (vr - vl) * 1e4 / dynamic_cast<const ServoDevConf*>(GetDevConfig())->
            servo_info().wheel_distance_10th1_mm();

        //speed
        odom.mutable_twist()->mutable_twist()->mutable_linear()->set_x(line);
        /*
         * default zero
        odom.mutable_twist()->mutable_twist()->mutable_linear()->set_y();
        odom.mutable_twist()->mutable_twist()->mutable_linear()->set_z();
        odom.mutable_twist()->mutable_twist()->mutable_angular()->set_x();
        odom.mutable_twist()->mutable_twist()->mutable_angular()->set_y();
        */
        odom.mutable_twist()->mutable_twist()->mutable_angular()->set_z(omg);
        odom.mutable_twist()->mutable_covariance()->Resize(36, 0.f);

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
        return frame_processor_sp_(std::make_shared
                <ventura::common_msgs::nav_msgs::Odometry>(odom));
    }

} //namespace parser
} //namespace chss
} //namespace mstf

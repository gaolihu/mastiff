/*
 * @Date: 2023-11-06 13:56:26
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/parser/parse_libs/imu_parser.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "modules/chassis/parser/parse_libs/imu_parser.h"

#include <sys/time.h>
#include <time.h>

namespace mstf {
namespace chss {
namespace parser {

    IMUParser::IMUParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "IMUParser construct";
        }

    IMUParser::~IMUParser() {
        AINFO << "IMUParser de-construct";
    }

    int IMUParser::Init() {
        auto dev = dynamic_cast<const ImuDevice&>(GetDevice());
        if (dev.has_uart_conf()) {
            return ParserBaseItf::Init(dev.sn_ind().port(),
                    &dev.uart_conf().buf_setting());
        } else if (dev.has_i2c_conf()) {
            return ParserBaseItf::Init(dev.sn_ind().port(),
                    &dev.i2c_conf().buf_setting());
        } else {
            AINFO << "Init IMU error!";
            return -1;
        }
    }

    int IMUParser::ParseRawBuffer(const uint8_t* buf,
            const size_t len) {
        int cbuf_len = cbuf_raw_->CbufSize();
        if (cbuf_len < 45)
            //cbuf data not enough
            return 0;

        std::vector<uint8_t> raw_data;
        raw_data.resize(cbuf_len);

        if (!cbuf_raw_->ReadSafely(raw_data)) {
            AFATAL << "read circular buffer for LD failed";
            return -1;
        }

        int head = 0;
        int tail = cbuf_len - 1;
#if 1
        std::ostringstream os;
        std::ostringstream oss;
#if 1
        for (int i = 0; i < cbuf_len; i++) {
            oss << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<int>(raw_data[i]) << " ";
        }
#endif
        os << "before parse, cbuf_len: " << cbuf_len <<
            //", cbuf consumed: " << cbuf_raw_->CbufSize() <<
            ", head: " << head <<
            ", tail: " << tail;

        AINFO << "1, imu data flow(" << os.str() <<
            ") cbuf raw: [ " << oss.str() << " ]";
#endif
        while ((tail - head + 1) >= (int)sizeof(IMUMsgData)) {
            if (raw_data[head] == 0xAA &&
                    raw_data[head + 1] == 0x00 &&
                    raw_data[head + 13] == 0x55) {
                //frame OK
                auto frame = (IMUMsgData *)&raw_data[head];
                uint8_t calc_sum =
                    raw_data[head + 2] +
                    raw_data[head + 3] +
                    raw_data[head + 4] +
                    raw_data[head + 5] +
                    raw_data[head + 6] +
                    raw_data[head + 7] +
                    raw_data[head + 8] +
                    raw_data[head + 9] +
                    raw_data[head + 10] +
                    raw_data[head + 11] +
                    raw_data[head + 12] +
                    raw_data[head + 13];

                if (frame->check_sum == calc_sum) {
                    ParseSigleFrame(frame);
                } else {
                    AWARN << "data checksum fail!";
                }

                head += sizeof(IMUMsgData);
                cbuf_raw_->Restore(sizeof(IMUMsgData));
            } else {
                //rewind head and find again
                head++;
                continue;
            }
        }
#if 0
#if 1
        oss.str("");
        for (int j = head; j <= tail; j++) {
            oss << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<int>(raw_data[j]) << " ";
        }
#endif
        os.str("");
        os << "parse end, cbuf_len: " << cbuf_len <<
            //", cbuf consumed: " << cbuf_raw_->CbufSize() <<
            ", head: " << head <<
            ", tail: " << tail;

        AINFO << "2, imu data flow(" << os.str() <<
            ") cbuf remain: [ " << oss.str() << " ]";
#endif
        return 0;
    }

    int IMUParser::ParseSigleFrame(const IMUMsgData* imu_msg) {
        float acc_x = (float)(imu_msg->data.acc_x) / g_acc_resolution;
        float yaw_angle =(float)(imu_msg->data.yaw_angle) * M_PI / (g_angle_resolution * 180.0);
        float yaw_rate = (float)(imu_msg->data.yaw_rate) * M_PI / (g_angle_resolution * 180.0);
        float roll_angle =(float)(imu_msg->data.roll_angle) * M_PI / (g_angle_resolution * 180.0);
        float pitch_angle =(float)(imu_msg->data.pitch_angle) * M_PI / (g_angle_resolution * 180.0);

        ADEBUG << yaw_angle;   // rad
        ADEBUG << yaw_rate;    // rad/s
        ADEBUG << roll_angle;  // rad
        ADEBUG << pitch_angle; // rad
        ADEBUG << acc_x;       // m/s
        ADEBUG << "--------------------------";
        // ADEBUG << "IMU single frame: " << data.size() << data[0] << data[1] << len;

        ventura::common_msgs::sensor_msgs::Imu imu_proto;
        timeval tv;
        gettimeofday(&tv, NULL);
        // header
        imu_proto.mutable_header()->set_seq(imu_proto.header().seq() + 1);
        imu_proto.mutable_header()->mutable_stamp()->set_sec(tv.tv_sec);
        imu_proto.mutable_header()->mutable_stamp()->set_nsec(tv.tv_usec * 1e3);
        imu_proto.mutable_header()->set_frame_id("imu_link");

        // covariance
        std::vector<double> identity{1, 0, 0,
                                     0, 1, 0,
                                     0, 0, 1};
        imu_proto.mutable_angular_velocity_covariance()->Add(identity.begin(), identity.end());
        imu_proto.mutable_linear_acceleration_covariance()->Add(identity.begin(), identity.end());
        imu_proto.mutable_orientation_covariance()->Add(identity.begin(), identity.end());

        // velocity
        imu_proto.mutable_linear_acceleration()->set_x(acc_x);
        imu_proto.mutable_linear_acceleration()->set_y(0.0);
        imu_proto.mutable_linear_acceleration()->set_z(0.0);

        imu_proto.mutable_angular_velocity()->set_x(yaw_rate);
        imu_proto.mutable_angular_velocity()->set_y(0.0);
        imu_proto.mutable_angular_velocity()->set_z(0.0);

        // orientation
        double cy = cos(yaw_angle * 0.5);
        double sy = sin(yaw_angle * 0.5);
        double cp = cos(pitch_angle * 0.5);
        double sp = sin(pitch_angle * 0.5);
        double cr = cos(roll_angle * 0.5);
        double sr = sin(roll_angle * 0.5);

        ventura::common_msgs::geometry_msgs::Quaternion q;
        q.set_w(cy * cp * cr + sy * sp * sr);
        q.set_x(cy * cp * sr - sy * sp * cr);
        q.set_y(sy * cp * sr + cy * sp * cr);
        q.set_z(sy * cp * cr - cy * sp * sr);
        imu_proto.mutable_orientation()->CopyFrom(q);

        if(frame_processor_ && (++frame_count_ % 2 == 0)) {
            return frame_processor_(&imu_proto,
                    "ventura::common_msgs::sensor_msgs::Imu");
        } else {
            return -1;
        }

        return 0;
    }

} //namespace parser
} //namespace chss
} //namespace mstf

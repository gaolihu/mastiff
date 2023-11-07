#include <cmath>

#include "modules/chassis/proto/frame_down_stream.pb.h"

#include "modules/chassis/parser/pack_libs/servo_packer.h"

#ifndef WHEEL_DISTANCE
#define WHEEL_DISTANCE 0.531 //m
#endif

#ifndef WHEEL_DIAMETER
#define WHEEL_DIAMETER 0.1411 //m
#endif

namespace mstf {
namespace chss {
namespace parser {

    ServoPacker::ServoPacker(const std::string& dev) :
        PackerBaseItf(dev) {
            AINFO << "ServoPacker construct for " << dev;
        }

    ServoPacker::~ServoPacker() {
        AINFO << "ServoPacker de-construct";
    }

    //canOpen speed control
    std::vector<std::tuple<const int, const std::vector<uint8_t>>>
    ServoPacker::PackMotorDiffSpeedDouble(const DownToServoData& sd) {
        std::vector<std::tuple<const int,
            const std::vector<uint8_t>>> tds;
#if 1
        AINFO << "packing diff speed, " <<
            "linear: " << sd.motor_speed().linear() <<
            "m/s, angular: " << sd.motor_speed().angular() <<
            "rad/s, dec/acc: " << sd.motor_speed().acc_dec();
#endif

        WheelRpm rpm;
        ConvertDiffSpeed2Rpm(sd.motor_speed(), rpm);

        FillDataContainerCanOpenAxleSpeed(tds,
                rpm.left_rpm() * 10, rpm.right_rpm() * 10);

        return tds;
    }

    //free can protocol for diff speed control
    std::tuple<const int, const std::vector<uint8_t>>
    ServoPacker::PackMotorDiffSpeed(const DownToServoData& sd) {
        int size = 0;
        int16_t addr[200] = {0};
        int value[200] = {0};
        std::vector<uint8_t> data;
#if 0
        AINFO << "packing diff speed, " <<
            "linear: " << sd.motor_speed().linear() <<
            "m/s, angular: " << sd.motor_speed().angular() <<
            "rad/s";
#endif
        config_servo_motor_wheel_speed(addr, value, &size);

        data.emplace_back(DS_MOTOR_WRITE_DIFF_SPEED);
        data.emplace_back(0x00);
        data.emplace_back(0x00);
        data.emplace_back(0x00);

        //line speed
        data.emplace_back((int)(sd.motor_speed().linear() * 1000) & 0xff);
        data.emplace_back(((int)(sd.motor_speed().linear() * 1000) & 0xff00) >> 8);

        //angulor speed
        data.emplace_back((int)(sd.motor_speed().angular() * 1000) & 0xff);
        data.emplace_back(((int)(sd.motor_speed().angular() * 1000) & 0xff00) >> 8);

        return {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data};
    }

    //free can for wheel speed control
    std::tuple<const int, const std::vector<uint8_t>>
    ServoPacker::PackMotorWheelSpeed(const DownToServoData& sd) {
        int size = 0;
        int16_t addr[200] = {0};
        int value[200] = {0};
        std::vector<uint8_t> data;

        WheelRpm rpm;
        ConvertDiffSpeed2Rpm(sd.motor_speed(), rpm);
#if 0
        AINFO << "packing wheel speed, RPM" <<
            " left :" << std::hex << rpm.left_rpm() <<
            ", right :" << std::hex << rpm.right_rpm();
#endif
        if (sd.motor_speed().has_wheel_reverse()) {
            config_servo_motor_wheel_reverse(addr, value, &size);

            data.emplace_back(DS_MOTOR_WRITE_BOTH_CMD);
            data.emplace_back(addr[0] & 0xff);
            data.emplace_back(((addr[0]) & 0xff00) >> 8);
            data.emplace_back(0x00);

            data.emplace_back(sd.motor_speed().wheel_reverse().value());
            data.emplace_back(0x00);
            data.emplace_back(0x00);
            data.emplace_back(0x00);

            return {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data};
        } else {
            config_servo_motor_wheel_speed(addr, value, &size);

            data.emplace_back(DS_MOTOR_WRITE_BOTH_CMD);
            data.emplace_back(addr[0] & 0xff);
            data.emplace_back(((addr[0]) & 0xff00) >> 8);
            data.emplace_back(0x00);

            //axle #1 RPM
            data.emplace_back(rpm.left_rpm() & 0xff);
            data.emplace_back((rpm.left_rpm() & 0xff00) >> 8);

            //axle #2 RPM
            data.emplace_back((~rpm.right_rpm() + 1) & 0xff);
            data.emplace_back(((~rpm.right_rpm() + 1) & 0xff00) >> 8);

            return {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data};
        }
    }

    std::vector<std::tuple<const int, const std::vector<uint8_t>>>
    ServoPacker::PackMotorMessageArrayRaw(const DownToServoData& sd) {
        uint8_t cmd[200] = {0};
        int16_t addr[200] = {0};
        uint8_t sub_id[200] = {0};
        int value[200] = {0};
        int size = 0;

        std::vector<std::tuple<const int,
            const std::vector<uint8_t>>> tds;

        if (sd.has_config()) {
            switch (sd.config().opt()) {
                case E_SUBDEV_OPTS_INIT:
                    init_servo_motor_fn_des();
                    init_servo_motor_dn_des();
                    init_servo_motor_pn_des();
                    break;
                case E_SUBDEV_OPTS_START:
                    config_servo_motor_init(addr, value, &size);
                    //config_servo_motor_canopen_enable(addr, value, &size);
                    //enable motor
                    if (!FillDataContainerFreeWrite(tds, "ENABLE", addr, value, size)) {
                        AERROR << "fill PN data-enable ERROR!";
                    }

                    FillDataContainerCanOpenActivate(tds);

                    size = 0;
                    memset(addr, 0, sizeof(addr));
                    memset(sub_id, 0, sizeof(sub_id));
                    memset(value, 0, sizeof(value));
                    config_servo_motor_canopen_mode(cmd, addr, sub_id, value, &size);
                    config_servo_motor_acc_moderate_time(cmd, addr, sub_id, value, &size);
                    config_servo_motor_tpdo_rpdo(cmd, addr, sub_id, value, &size);

                    if (!FillDataContainerCanOpenConfig(tds, cmd, addr, sub_id, value, size)) {
                        AERROR << "fill canOpen speed zero ERROR!";
                    }
                    FillDataContainerCanOpenSync(tds);
                    FillDataContainerCanOpenResetStatus(tds);
                    FillDataContainerCanOpenAxleSpeed(tds, 0, 0);
                    FillDataContainerCanOpenAxleDisable(tds);
                    break;
                case E_SUBDEV_OPTS_ENABLE:
                    //stop
                    FillDataContainerCanOpenResetStatus(tds);
                    FillDataContainerCanOpenAxleSpeed(tds, 0, 0);
                    FillDataContainerCanOpenAxleEnable(tds);
                    FillDataContainerCanOpenStartReporting(tds);
                    break;
                case E_SUBDEV_OPTS_DISABLE:
                    FillDataContainerCanOpenAxleDisable(tds);
                    FillDataContainerCanOpenResetStatus(tds);
                    FillDataContainerCanOpenAxleSpeed(tds, 0, 0);
                    FillDataContainerCanOpenStopReporting(tds);
                    break;
                case E_SUBDEV_OPTS_STOP:
#if 0
                    show_all_servo_motor_fn_des();
                    show_all_servo_motor_dn_des();
                    show_all_servo_motor_pn_des();
#endif
                    FillDataContainerCanOpenAxleSpeed(tds, 0, 0);
                    break;
                case E_SUBDEV_OPTS_CLOSE:
#if 0
                    //free can protocol
                    config_servo_motor_disable(addr, value, &size);
                    //stop motor
                    if (!FillDataContainerFreeWrite(tds, "CLOSE", addr, value, size)) {
                        AERROR << "fill PN data-close ERROR!";
                    }
#endif
                    FillDataContainerCanOpenStopReporting(tds);
                    break;
                case E_SUBDEV_OPTS_STORE:
                    get_servo_motor_all_fn_addrs(addr, &size);
                    //query all FN registers
                    if (!FillDataContainerFreeRead(tds, "FN", addr, value, size)) {
                        AERROR << "fill FN data-store ERROR!";
                    }

                    get_servo_motor_all_dn_addrs(addr, &size);
                    //query all DN registers
                    if (!FillDataContainerFreeRead(tds, "DN", addr, value, size)) {
                        AERROR << "fill DN data-store ERROR!";
                    }

                    get_servo_motor_all_pn_addrs(addr, &size);
                    //query all PN registers
                    if (!FillDataContainerFreeRead(tds, "PN", addr, value, size)) {
                        AERROR << "fill PN data-store ERROR!";
                    }
                    break;
                default:
                    AERROR << "option TODO for " << sd.config().opt();
            }
        } else if (sd.has_motor_speed()) {
            AERROR << "wheel speed conversion TODO!";
        }

        return tds;
    }

    ///////////// Tesing /////////////
    const std::vector<uint8_t>
    ServoPacker::PackMotorMessageString(const
            DownToServoData& sd) {
        std::vector<uint8_t> servo_str;

        if (sd.has_motor_speed()) {
            WheelRpm rpm;
            ConvertDiffSpeed2Rpm(sd.motor_speed(), rpm);
#if 0
            AINFO << "string packing, RPM" <<
                " left :" << std::hex << rpm.left_rpm() <<
                ", right :" << std::hex << rpm.right_rpm();
#endif

            //ID
            servo_str.emplace_back('6');
            servo_str.emplace_back('0');
            servo_str.emplace_back('1');

            //
            servo_str.emplace_back('#');

            //write two axle cmd
            servo_str.emplace_back('e');
            servo_str.emplace_back('9');

            //address for setting RPM
            servo_str.emplace_back('1');
            servo_str.emplace_back('8');
            servo_str.emplace_back('2');
            servo_str.emplace_back('3');

            //sub index
            servo_str.emplace_back('0');
            servo_str.emplace_back('0');

            //axle #1 RPM
            servo_str.emplace_back(((rpm.left_rpm() & 0xf0) >> 4) > 9 ?
                    (((rpm.left_rpm() & 0xf0) >> 4) + 0x37) :
                    (((rpm.left_rpm() & 0xf0) >> 4) + 0x30));
            servo_str.emplace_back(((rpm.left_rpm() & 0xf) >> 0) > 9 ?
                    (((rpm.left_rpm() & 0xf) >> 0) + 0x37) :
                    (((rpm.left_rpm() & 0xf) >> 0) + 0x30));
            servo_str.emplace_back(((rpm.left_rpm() & 0xf000) >> 12) > 9 ?
                    (((rpm.left_rpm() & 0xf000) >> 12) + 0x37) :
                    (((rpm.left_rpm() & 0xf000) >> 12) + 0x30));
            servo_str.emplace_back(((rpm.left_rpm() & 0xf00) >> 8) > 9 ?
                    (((rpm.left_rpm() & 0xf00) >> 8) + 0x37) :
                    (((rpm.left_rpm() & 0xf00) >> 8) + 0x30));

            //axle #2 RPM
            servo_str.emplace_back(((rpm.right_rpm() & 0xf0) >> 4) > 9 ?
                    (((rpm.right_rpm() & 0xf0) >> 4) + 0x37) :
                    (((rpm.right_rpm() & 0xf0) >> 4) + 0x30));
            servo_str.emplace_back(((rpm.right_rpm() & 0xf) >> 0) > 9 ?
                    (((rpm.right_rpm() & 0xf) >> 0) + 0x37) :
                    (((rpm.right_rpm() & 0xf) >> 0) + 0x30));
            servo_str.emplace_back(((rpm.right_rpm() & 0xf000) >> 12) > 9 ?
                    (((rpm.right_rpm() & 0xf000) >> 12) + 0x37) :
                    (((rpm.right_rpm() & 0xf000) >> 12) + 0x30));
            servo_str.emplace_back(((rpm.right_rpm() & 0xf00) >> 8) > 9 ?
                    (((rpm.right_rpm() & 0xf00) >> 8) + 0x37) :
                    (((rpm.right_rpm() & 0xf00) >> 8) + 0x30));
        }

        return servo_str;
    }

    void ServoPacker::ConvertDiffSpeed2Wheel(const
            ServoSpeed& ds, WheelSpeed& ws) {
        ws.set_left_whl((ds.linear() - WHEEL_DISTANCE *
                    ds.angular() / 2));
        ws.set_right_whl((ds.linear() + WHEEL_DISTANCE *
                    ds.angular() / 2));
    }

    void ServoPacker::ConvertDiffSpeed2Rpm(const
            ServoSpeed& ds, WheelRpm& wr) {
        float vl = 0.f, vr = 0.f;
#if 1
        if (ds.acc_dec() != 0) {
            //accelerate / decelerate
            if (std::abs(std::abs(line_last_) + ds.acc_dec()) < 0.4) {
                if ((line_last_ > 0 && ds.acc_dec() > 0) ||
                    //+ acc
                        (line_last_ > 0 && ds.acc_dec() < 0)) {
                    //+ dec
                    vl = (line_last_ + ds.acc_dec() - WHEEL_DISTANCE *
                            angular_last_ / 2);
                    vr = (line_last_ + ds.acc_dec() + WHEEL_DISTANCE *
                            angular_last_ / 2);
                } else if ((line_last_ < 0 && ds.acc_dec() > 0)) {
                    //- acc
                    vl = (line_last_ - ds.acc_dec() - WHEEL_DISTANCE *
                            angular_last_ / 2);
                    vr = (line_last_ - ds.acc_dec() + WHEEL_DISTANCE *
                            angular_last_ / 2);
                } else if ((line_last_ < 0 && ds.acc_dec() < 0)) {
                    //- dec
                    vl = (line_last_ + ds.acc_dec() - WHEEL_DISTANCE *
                            angular_last_ / 2);
                    vr = (line_last_ + ds.acc_dec() + WHEEL_DISTANCE *
                            angular_last_ / 2);
                }

                wr.set_left_rpm((int)60 * vl / (M_PI * WHEEL_DIAMETER));
                wr.set_right_rpm((int)60 * vr / (M_PI * WHEEL_DIAMETER));

                line_last_ += ds.acc_dec();

                AWARN << "vehicle acc/dec: " << ds.acc_dec() <<
                    ", dest speed: " << line_last_ << "m/s";
            } else {
                AWARN << "SPEED TOO FAST!!! DANGEROUS!!! IGNORE!!!";
                vl = (line_last_ - WHEEL_DISTANCE *
                        angular_last_ / 2);
                vr = (line_last_ + WHEEL_DISTANCE *
                        angular_last_ / 2);
                wr.set_left_rpm((int)60 * vl / (M_PI * WHEEL_DIAMETER));
                wr.set_right_rpm((int)60 * vr / (M_PI * WHEEL_DIAMETER));
            }
        } else {
            vl = (ds.linear() - WHEEL_DISTANCE *
                    ds.angular() / 2);
            vr = (ds.linear() + WHEEL_DISTANCE *
                    ds.angular() / 2);
            wr.set_left_rpm((int)60 * vl / (M_PI * WHEEL_DIAMETER));
            wr.set_right_rpm((int)60 * vr / (M_PI * WHEEL_DIAMETER));

            line_last_ = ds.linear();
            angular_last_ = ds.angular();
        }
#else
        vl = (ds.linear() - WHEEL_DISTANCE *
                ds.angular() / 2);
        vr = (ds.linear() + WHEEL_DISTANCE *
                ds.angular() / 2);
        wr.set_left_rpm((int)60 * vl / (M_PI * WHEEL_DIAMETER));
        wr.set_right_rpm((int)60 * vr / (M_PI * WHEEL_DIAMETER));
#endif
    }

} //namespace parser
} //namespace chss
} //namespace mstf

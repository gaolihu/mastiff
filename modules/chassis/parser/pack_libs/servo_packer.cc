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

    //default for speed control
    std::tuple<const int, const std::vector<uint8_t>>
    ServoPacker::PackMotorMessageRaw(const DownToServoData& sd) {
        std::vector<uint8_t> data;

        WheelRpm rpm;
        ConvertDiffSpeed2Rpm(sd.diff_speed(), rpm);

#if 1
        AINFO << "raw packing, RPM" <<
            " left :" << std::hex << rpm.left_rpm() <<
            ", right :" << std::hex << rpm.right_rpm();
        show_all_servo_motor_dn_des();
        show_all_servo_motor_pn_des();
#endif

        data.emplace_back(DS_MOTOR_WRITE_BOTH_CMD);
        //data.emplace_back(f_3_0x18.ax0.func_addr & 0xff);
        //data.emplace_back(((f_3_0x18.ax0.func_addr) & 0xff00) >> 8);
        data.emplace_back(0x00);

        //axle #1 RPM
        data.emplace_back(rpm.left_rpm() & 0xff);
        data.emplace_back((rpm.left_rpm() & 0xff00) >> 8);

        //axle #2 RPM
        data.emplace_back(rpm.right_rpm() & 0xff);
        data.emplace_back((rpm.right_rpm() & 0xff00) >> 8);
        return {DS_MOTOR_DRIVE_MOTOR_ID, data};
    }

    std::vector<std::tuple<const int, const std::vector<uint8_t>>>
    ServoPacker::PackMotorMessageArrayRaw(const DownToServoData& sd) {
        std::vector<std::tuple<const int,
            const std::vector<uint8_t>>> tds;

        if (sd.has_config()) {
            if (sd.config().opt() == E_SERVO_OPTS_INIT) {
                init_servo_motor_fn_des();
                init_servo_motor_dn_des();
                init_servo_motor_pn_des();
            } else if (sd.config().opt() == E_SERVO_OPTS_START) {
                int size = 0;
                int addr[200] = {0};

                get_servo_motor_all_fn_addrs(addr, &size);
                //query all FN registers
                if (!FillDataContainer(tds, "FN", addr, size)) {
                    AERROR << "fill FN data ERROR!";
                }

                get_servo_motor_all_dn_addrs(addr, &size);
                //query all DN registers
                if (!FillDataContainer(tds, "DN", addr, size)) {
                    AERROR << "fill DN data ERROR!";
                }

                get_servo_motor_all_pn_addrs(addr, &size);
                //query all PN registers
                if (!FillDataContainer(tds, "PN", addr, size)) {
                    AERROR << "fill PN data ERROR!";
                }
            } else if (sd.config().opt() == E_SERVO_OPTS_STORE) {
                show_all_servo_motor_fn_des();
                show_all_servo_motor_dn_des();
                show_all_servo_motor_pn_des();
            }
        } else if (sd.has_whl_speed()) {
            AERROR << "wheel speed conversion TODO!";
        }

        return tds;
    }

    //Tesing
    const std::vector<uint8_t>
    ServoPacker::PackMotorMessageString(const
            DownToServoData& sd) {
        std::vector<uint8_t> servo_str;

        if (sd.has_diff_speed()) {
            WheelRpm rpm;
            ConvertDiffSpeed2Rpm(sd.diff_speed(), rpm);

            AINFO << "string packing, RPM" <<
                " left :" << std::hex << rpm.left_rpm() <<
                ", right :" << std::hex << rpm.right_rpm();

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
        } else if (sd.has_whl_speed()) {
        }

        return servo_str;
    }

    void ServoPacker::ConvertDiffSpeed2Wheel(const
            DiffSpeed& ds, WheelSpeed& ws) {
        ws.set_left_whl((ds.linear() - WHEEL_DISTANCE *
                    ds.palstance() / 2) * 1000);
        ws.set_right_whl((ds.linear() + WHEEL_DISTANCE *
                    ds.palstance() / 2) * 1000);
    }

    void ServoPacker::ConvertDiffSpeed2Rpm(const
            DiffSpeed& ds, WheelRpm& wr) {
        float vl = (ds.linear() - WHEEL_DISTANCE *
                ds.palstance() / 2) * 1000;
        float vr = (ds.linear() + WHEEL_DISTANCE *
                ds.palstance() / 2) * 1000;
        wr.set_left_rpm((int)60 * vl / (M_PI * WHEEL_DIAMETER));
        wr.set_right_rpm((int)60 * vr / (M_PI * WHEEL_DIAMETER));
    }

} //namespace parser
} //namespace chss
} //namespace mstf

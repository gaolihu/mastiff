#include "cyber/cyber.h"

#include "modules/chassis/proto/input_output_chs.pb.h"

#include "modules/chassis/devices/key_simulate.h"

namespace mstf {
namespace chss {
namespace device {

    KeySimulate::KeySimulate() {
        AINFO << "KeySimulate construct";
    }

    KeySimulate::~KeySimulate() {
        AINFO << "KeySimulate de-construct";
        tty_stop();
    }

    void KeySimulate::StartSimulate() {
        AINFO << "start catching simulative keys!";

        auto fc = [&](int ev)->int {
            int cmd = ev & 0xffff;
            int val = (ev & 0xffff0000) >> 16;

            if (ev != 0) {
                AINFO << "key handle, command key: " <<
                    std::hex << cmd <<
                    ", value: " << val;

                std::shared_ptr<ChassisCtrl> chassis_ctrl = std::make_shared<ChassisCtrl>();

                switch (cmd) {
                    //////////////////////////////AUDIO TEST//////////////////////////////////
                case 0x0001: {
                    //play keyboard choosed music
                    chassis_ctrl->mutable_soc_ctrl()->mutable_audio()->mutable_play()->set_audio_id(val);
                    chassis_ctrl->mutable_soc_ctrl()->mutable_audio()->mutable_play()->mutable_pack()->set_value("Chinese");
                    chassis_ctrl->mutable_soc_ctrl()->mutable_audio()->mutable_play()->mutable_play_cnt()->set_value(val);
                    chassis_ctrl->mutable_soc_ctrl()->mutable_audio()->mutable_play()->mutable_interrupt()->set_value(true);

                    chassis_ctrl->mutable_soc_ctrl()->mutable_audio()->mutable_vol()->set_volmue(35);
                    chassis_ctrl->mutable_soc_ctrl()->mutable_audio()->mutable_vol()->mutable_mute_sw()->set_value(false);

                    chassis_ctrl->mutable_soc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("ver1.9");
                }
                    break;

                case 0x4b49: {
                    // i, get ip
                    proto::WirelessInfoType get_type=proto::WirelessInfoType::WIFI_GET_IP;
                    chassis_ctrl->mutable_soc_ctrl()->mutable_wireless()->set_get_wifi_info(get_type);
                    // f, reconnect wifi
                    // proto::WirelessInfoType get_type = proto::WirelessInfoType::WIFI_GET_RECONNECT;
                    // chassis_ctrl->mutable_soc_ctrl()->mutable_wireless()->set_get_wifi_info(get_type);
                }
                    break;
                case 0x4b51: {
                    // p, capture image picture
                    chassis_ctrl->mutable_soc_ctrl()->mutable_camera()->set_capture(true);
                }
                    break;
                case 0x4b55: {
                    // v, image video
                    chassis_ctrl->mutable_soc_ctrl()->mutable_camera()->set_video(true);
                }
                    break;

                case 0x0003: {
                }
                    break;
                    //////////////////////////////AUDIO TEST//////////////////////////////////

                    //////////////////////////////SPEED CONTROL//////////////////////////////////
                    static float line_speed = 0.06;   // m/s
                    static float angular_speed = 0.25;   // rad/s
                case 0x5b41:
                    //↑
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(line_speed);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;
                case 0x4b41:
                    //w, enable
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(0.05);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;

                case 0x5b44:
                    //← wheel speed
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(angular_speed);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;
                case 0x4b44:
                    //a, left, use diff speed
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(0.01);
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(0.2);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;

                case 0x5b43:
                    //→ wheel speed
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(-angular_speed);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;
                case 0x4b43:
                    //d, right
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(-0.01);
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(-0.2);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;

                case 0x5b42:
                    //↓, wheel speed
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(-line_speed);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;
                case 0x4b42:
                    //s, backward
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(-0.04);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;

                case 0x4b45:
                    //e, speed 0 stop
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(0);
                    chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
                    break;
                case 0x4b46:
                    //t, stop, and stop release data
                    chassis_ctrl->mutable_speed_ctrl()->mutable_release_wheels()->set_value(false);
                    break;
                case 0x4b60:
                    //f, enable
                    chassis_ctrl->mutable_speed_ctrl()->mutable_release_wheels()->set_value(true);
                    break;
                case 0x4b58:
                    //g, reverse wheels 1
                    //chassis_ctrl->mutable_speed_ctrl()->mutable_wheel_reverse()->set_value(true);
                    //
                    //g, decelerate
                    //chassis_ctrl->mutable_speed_ctrl()->set_acc_dec(-0.01);
                    //
                    //g, decrease linear speed by 0.01
                    line_speed -= 0.01;
                    if (line_speed < 0)
                        line_speed = 0;

                    AINFO << "decrease linear speed, current speed: " << line_speed;
                    break;
                case 0x4b61:
                    //h, reverse wheels 0
                    //chassis_ctrl->mutable_speed_ctrl()->mutable_wheel_reverse()->set_value(false);
                    //
                    //h, accelerate
                    //chassis_ctrl->mutable_speed_ctrl()->set_acc_dec(0.01);
                    //
                    //h, decrease linear speed by 0.01
                    line_speed += 0.01;
                    if (line_speed > 0.5) {
                        line_speed = 0.1;
                        AWARN << "set speed too fast!!";
                    }
                    AINFO << "increase linear speed, current speed: " << line_speed;
                    //////////////////////////////SPEED CONTROL//////////////////////////////////
                    break;

                default:
                    AWARN << "Nothing to do!";
                    break;
                }

                if (chassis_ctrl != nullptr) {
                    if (upper_handler_) {
                        upper_handler_(chassis_ctrl);
                    } else {
                        AWARN << "no handler for cmd: " << cmd;
                    }

                    chassis_ctrl->Clear();
                    chassis_ctrl.reset();
                }
            }

            return 0;
        };

        tty_register_handle("Key Control <<< Downstream", fc, 50);
    }

} // namespace device
} // namespace chss
} // namespace mstf

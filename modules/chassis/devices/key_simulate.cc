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
        AINFO << "Start catching key simulation!";

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

                case 0x0002: {
                }
                    break;

                case 0x0003: {
                }
                    break;
                    //////////////////////////////AUDIO TEST//////////////////////////////////

                    //////////////////////////////SPEED CONTROL//////////////////////////////////
                case 0x5b41:
                    //¡ü
                case 0x4b41: {
                    //w, forward
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(0.15);
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(0);
                }
                    break;
                case 0x5b44:
                    //¡û
                case 0x4b44: {
                    //a, left
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(0);
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(1.5);
                }
                    break;
                case 0x5b43:
                    //¡ú
                case 0x4b43: {
                    //d, right
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(0);
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(-1.5);
                }
                    break;
                case 0x5b42:
                    //¡ý, backward
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(-0.15);
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(0);
                case 0x4b42: {
                    //s, backward
                    chassis_ctrl->mutable_speed_ctrl()->set_linear(0);
                    chassis_ctrl->mutable_speed_ctrl()->set_angular(0);
                }
                    break;
                    //////////////////////////////SPEED CONTROL//////////////////////////////////

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

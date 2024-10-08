#include "cyber/cyber.h"

#include "modules/chassis/proto/chss_io.pb.h"

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

                std::shared_ptr<MiscChassisCtrl> chassis_ctrl = std::make_shared<MiscChassisCtrl>();

                switch (cmd) {
                case 0x0001: {
                    //play music
                    AudioPlay(chassis_ctrl, val);
                }
                    break;

                case 0x00002: {
                    //audio setting
                    AudioCtrl(chassis_ctrl, val);
                }
                    break;

                case 0x00003: {
                    //device manager
                    DeviceManageSim(chassis_ctrl, val);
                }
                    break;
                case 0x00004: {
                    //wireless manage
                    WirelessKeySim(chassis_ctrl, val);
                }
                    break;
                case 0x00005: {
                    CameraKeySim(chassis_ctrl, val);
                }
                    break;

                case 0x4b49: {
                }
                    break;
                case 0x4b51: {
                }
                    break;
                case 0x4b55: {
                }
                    break;

                    //////////////////////////////AUDIO TEST//////////////////////////////////

                    //////////////////////////////SPEED CONTROL//////////////////////////////////
                    static float line_speed = 0.06;   // m/s
                    static float angular_speed = 0.25;   // rad/s
                case 0x5b41:
                    //↑
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed);
                    break;
                case 0x4b41:
                    //w, enable
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed / 2);
                    break;

                case 0x5b44:
                    //← wheel speed
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_angular(angular_speed);
                    break;
                case 0x4b44:
                    //a, left, use diff speed
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed / 2);
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_angular(angular_speed / 2);
                    break;

                case 0x5b43:
                    //→ wheel speed
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_angular(-angular_speed);
                    break;
                case 0x4b43:
                    //d, right
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(-line_speed / 2);
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_angular(-angular_speed / 2);
                    break;

                case 0x5b42:
                    //↓, wheel speed
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(-line_speed);
                    break;
                case 0x4b42:
                    //s, backward
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(-line_speed / 2);
                    break;

                case 0x4b45:
                    //e, speed 0 stop
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(0);
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_angular(0);
                    break;
                case 0x4b46:
                    //t, stop, and stop release data
                    chassis_ctrl->mutable_move_ctrl()->mutable_wheel_release()->set_value(false);
                    break;
                case 0x4b60:
                    //f, enable
                    chassis_ctrl->mutable_move_ctrl()->mutable_wheel_release()->set_value(true);
                    break;
                case 0x2b:
                    //+
                    line_speed += 0.01;
                    if (line_speed > 0.5) {
                        line_speed = 0.1;
                        AWARN << "set speed too fast!!";
                    }
                    AINFO << "increase linear speed, current speed: " << line_speed;
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed);
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_angular(angular_speed);
                    break;
                case 0x2d:
                    //-
                    line_speed -= 0.01;
                    if (line_speed < 0)
                        line_speed = 0;
                    AINFO << "decrease linear speed, current speed: " << line_speed;
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed);
                    chassis_ctrl->mutable_move_ctrl()->mutable_diff_spd()->set_angular(angular_speed);
                    break;
                case 0x4b58:
                    //g, reverse wheels 1
                    chassis_ctrl->mutable_move_ctrl()->mutable_wheel_reverse()->set_value(true);
                    break;
                    //////////////////////////////SPEED CONTROL//////////////////////////////////

                    //////////////////////////////QUIT PROGRAM//////////////////////////////////
                case 0x71:
                    //q, quit the program
                    //recycle action TBF
                    exit(0);
                    break;
                    //////////////////////////////QUIT PROGRAM//////////////////////////////////

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

    void KeySimulate::DeviceManageSim(std::shared_ptr<MiscChassisCtrl>&
            chassis_ctrl, const int val) {
#if 0
        switch (val) {
                // --------- GPIO --------- //
            case 11:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_gpio_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 12:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_gpio_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 13:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_gpio_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 14:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_gpio_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- MCU0 --------- //
            case 21:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 22:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 23:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 24:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- MCU1 --------- //
            case 31:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 32:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 33:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 34:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_mcu_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- MOTOR --------- //
            case 41:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_servo_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 42:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_servo_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 43:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_servo_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 44:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_servo_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- LIDAR --------- //
            case 51:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_lidar_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 52:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_lidar_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 53:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_lidar_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 54:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_lidar_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- CAMERA0 --------- //
            case 61:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 62:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 63:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 64:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- CAMERA1 --------- //
            case 71:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 72:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 73:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 74:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- CAMERA2 --------- //
            case 81:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(2);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 82:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(2);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 83:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(2);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 84:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(2);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- CAMERA3 --------- //
            case 91:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(3);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 92:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(3);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 93:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(3);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 94:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(3);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- CAMERA4 --------- //
            case 101:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(4);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 102:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(4);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 103:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(4);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 104:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(4);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- AUDIO --------- //
            case 111:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_audio_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 112:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_audio_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 113:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_audio_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 114:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_audio_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

            case 121:
                // --------- LCD --------- //
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_lcd_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 122:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_lcd_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 123:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_lcd_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 124:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_lcd_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- WIFI --------- //
            case 131:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_wifi_4g_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 132:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_wifi_4g_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 133:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_wifi_4g_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 134:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_wifi_4g_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- 4G --------- //
            case 141:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_wifi_4g_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 142:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_wifi_4g_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 143:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_wifi_4g_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 144:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_camera_m()->set_index(1);
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_wifi_4g_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

                // --------- IMU --------- //
            case 151:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_imu_m()->set_val(E_DEVICES_MANAGE_START);
                break;
            case 152:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_imu_m()->set_val(E_DEVICES_MANAGE_STOP);
                break;
            case 153:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_imu_m()->set_val(E_DEVICES_MANAGE_RESUME);
                break;
            case 154:
                chassis_ctrl->mutable_misc_ctrl()->mutable_dev_maganger()->mutable_imu_m()->set_val(E_DEVICES_MANAGE_CLOSE);
                break;

            default:
                AWARN << "unspported device manage command!";
                break;
        }
#endif
    }

void KeySimulate::WirelessKeySim(std::shared_ptr<MiscChassisCtrl>& chassis_ctrl, const int val)
{
    EE_WIFI_FUNCTIONS get_type = E_WIFI_GET_IP;
    switch (val)
    {
    case 1:
        get_type = E_WIFI_GET_MODE;
        break;
    case 2:
        get_type = E_WIFI_GET_IP;
        break;
    case 3:
        get_type = E_WIFI_GET_MAC;
        break;
    case 4:
        get_type = E_WIFI_GET_DNS;
        break;
    case 5:
        get_type = E_WIFI_GET_WPA_INFO;
        break;
    case 6:
        get_type = E_WIFI_GET_NAME;
        break;
    case 7:
        get_type = E_WIFI_GET_ROUTER_MAC;
        break;
    case 8:
        get_type = E_WIFI_GET_STATUS;
        break;
    case 9:
        get_type = E_WIFI_GET_SCAN_LIST;
        break;
    case 10:
        get_type = E_WIFI_GET_RECONNECT;
        break;
    case 11:
        get_type = E_WIFI_DISCONNECT;
        break;
    case 12:
        get_type = E_WIFI_CHECK_CONNECTION;
        break;
    case 13:
        get_type = E_WIFI_TEST_SPEED;
        break;
    case 14: {
        get_type = E_WIFI_CONNECT;
        chassis_ctrl->mutable_misc_ctrl()->mutable_wifi()->mutable_auth_info()->mutable_ssid()->set_value("AITO");
        chassis_ctrl->mutable_misc_ctrl()->mutable_wifi()->mutable_auth_info()->mutable_pswd()->set_value("wjtswjts");
    }
        break;
    case 15: {
        get_type = E_WIFI_CREATE_AP;
        chassis_ctrl->mutable_misc_ctrl()->mutable_wifi()->mutable_auth_info()->mutable_ssid()->set_value("aventurier");
        chassis_ctrl->mutable_misc_ctrl()->mutable_wifi()->mutable_auth_info()->mutable_pswd()->set_value("12345678");
    }
        break;
    case 16:
        get_type = E_WIFI_CLOSE; // 关闭 wifi功能
        break;
    case 17:
        get_type = E_WIFI_START; // 开启 wifi 功能
        break;
    default:
        break;
    }
    chassis_ctrl->mutable_misc_ctrl()->mutable_wifi()->set_wifi_func(get_type);
}

void KeySimulate::AudioPlay(std::shared_ptr<MiscChassisCtrl>& ctrl, const int val) {
    ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_play()->set_sound_id(val);
}

void KeySimulate::AudioCtrl(std::shared_ptr<MiscChassisCtrl>& ctrl, const int val) {
    switch (val) {
        case 0:
            //set mute
            ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_vol()->mutable_mute_sw()->set_value(true);
            break;
        case 1:
            ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_vol()->set_volume(0);
            break;
        case 2: // 音量 50%
            ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_vol()->set_volume(50);
            break;
        case 3: // 音量 100%
            ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_vol()->set_volume(100);
            break;
        case 11: // 中文
            ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("chinese");
            break;
        case 12: // 英文
            ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("english");
            break;
        case 13: // 日语(小日本)
            ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("xiaoriben");
            break;
        case 14: // 韩语
            ctrl->mutable_misc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("korean");
            break;
        default:
            break;
    }
}

void KeySimulate::CameraKeySim(std::shared_ptr<MiscChassisCtrl>& ctrl, const int val)
{
#if 0
    switch (val)
    {
    case 0:
        // capture a frame save to file
        ctrl->mutable_misc_ctrl()->mutable_camera()->set_capture(true);
    break;
    case 1:
        //TODO: record images as a video
        // chassis_ctrl->mutable_misc_ctrl()->mutable_camera()->set_video(true);
    break;
    case 2:
        // close all cameras
        ctrl->mutable_misc_ctrl()->mutable_camera()->set_close(true);
    break;
    case 3:
        // restart all cameras
        ctrl->mutable_misc_ctrl()->mutable_camera()->set_restart(true);
    break;
    default:
        break;
    }
#endif
}

} // namespace device
} // namespace chss
} // namespace mstf

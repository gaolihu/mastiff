#include "cyber/cyber.h"

#include "modules/cambrian/brain/bizlogic/key_simulate.h"

namespace mstf {
namespace camb {
namespace brain {

    KeySimulate::KeySimulate() {
        AINFO << "KeySimulate construct";
        chassis_ctrl_ = std::make_shared<MiscChassisCtrl>();
        mission_setting_ = std::make_shared<MissionCommand>();
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

                static float line_speed = 0.06;   // m/s
                static float angular_speed = 0.25;   // rad/s

                //////////////////////////////CHASSIS CONTROL//////////////////////////////////
                switch (cmd) {
                case 0x0001: {
                    //play music
                    AudioPlay(val);
                }
                    break;

                case 0x00002: {
                    //audio setting
                    AudioCtrl(val);
                }
                    break;

                case 0x00003: {
                    //device manager
                    DeviceManage(val);
                }
                    break;
                case 0x00004: {
                    //wireless manage
                    WirelessSetting(val);
                }
                    break;
                case 0x00005: {
                    CameraSetting(val);
                }
                    break;
                //////////////////////////////CHASSIS CONTROL//////////////////////////////////

                //////////////////////////////MISSION SETTING//////////////////////////////////
                case 0x00006: {
                    MissionSetting(val);
                }
                    break;
                //////////////////////////////MISSION SETTING//////////////////////////////////

                    //////////////////////////////SPEED CONTROL//////////////////////////////////
                case 0x5b41:
                    //↑
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed);
                    break;
                case 0x4b41:
                    //w, enable
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed / 2);
                    break;

                case 0x5b44:
                    //← wheel speed
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_angular(angular_speed);
                    break;
                case 0x4b44:
                    //a, left, use diff speed
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed / 2);
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_angular(angular_speed / 2);
                    break;

                case 0x5b43:
                    //→ wheel speed
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_angular(-angular_speed);
                    break;
                case 0x4b43:
                    //d, right
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(-line_speed / 2);
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_angular(-angular_speed / 2);
                    break;

                case 0x5b42:
                    //↓, wheel speed
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(-line_speed);
                    break;
                case 0x4b42:
                    //s, backward
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(-line_speed / 2);
                    break;

                case 0x4b45:
                    //e, speed 0 stop
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(0);
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_angular(0);
                    break;
                case 0x4b46:
                    //t, stop, and stop release data
                    chassis_ctrl_->mutable_move_ctrl()->mutable_wheel_release()->set_value(false);
                    break;
                case 0x4b60:
                    //f, enable
                    chassis_ctrl_->mutable_move_ctrl()->mutable_wheel_release()->set_value(true);
                    break;
                case 0x2b:
                    //+
                    line_speed += 0.01;
                    if (line_speed > 0.5) {
                        line_speed = 0.1;
                        AWARN << "set speed too fast!!";
                    }
                    AINFO << "increase linear speed, current speed: " << line_speed;
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed);
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_angular(angular_speed);
                    break;
                case 0x2d:
                    //-
                    line_speed -= 0.01;
                    if (line_speed < 0)
                        line_speed = 0;
                    AINFO << "decrease linear speed, current speed: " << line_speed;
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_linear(line_speed);
                    chassis_ctrl_->mutable_move_ctrl()->mutable_diff_spd()->set_angular(angular_speed);
                    break;
                case 0x4b58:
                    //g, reverse wheels 1
                    chassis_ctrl_->mutable_move_ctrl()->mutable_wheel_reverse()->set_value(true);
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

                if (chassis_ctrl_->ByteSizeLong()) {
                    if (upper_handler_) {
                        upper_handler_(chassis_ctrl_);
                    } else {
                        AWARN << "no chss ctrl handler for cmd: " << cmd;
                    }

                    chassis_ctrl_->Clear();
                }

                if (mission_setting_->ByteSizeLong()) {
                    if (upper_handler_) {
                        upper_handler_(mission_setting_);
                    } else {
                        AWARN << "no mission setting handler for cmd: " << cmd;
                    }

                    mission_setting_->Clear();
                }
            }

            return 0;
        };

        tty_register_handle("Key Control <<< Downstream", fc, 50);
    }

    void KeySimulate::DeviceManage(const int val) {
    }

    void KeySimulate::WirelessSetting(const int val)
    {
        EE_WIFI_FUNCTIONS get_type = E_WIFI_GET_IP;
        switch (val) {
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
                chassis_ctrl_->mutable_misc_ctrl()->mutable_wifi()->mutable_auth_info()->mutable_ssid()->set_value("AITO");
                chassis_ctrl_->mutable_misc_ctrl()->mutable_wifi()->mutable_auth_info()->mutable_pswd()->set_value("wjtswjts");
            }
                break;
            case 15: {
                get_type = E_WIFI_CREATE_AP;
                chassis_ctrl_->mutable_misc_ctrl()->mutable_wifi()->mutable_auth_info()->mutable_ssid()->set_value("aventurier");
                chassis_ctrl_->mutable_misc_ctrl()->mutable_wifi()->mutable_auth_info()->mutable_pswd()->set_value("12345678");
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
        chassis_ctrl_->mutable_misc_ctrl()->mutable_wifi()->set_wifi_func(get_type);
    }

    void KeySimulate::AudioPlay(const int val) {
        chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_play()->set_sound_id(val);
    }

    void KeySimulate::AudioCtrl(const int val) {
        switch (val) {
            case 0:
                //set mute
                chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_vol()->mutable_mute_sw()->set_value(true);
                break;
            case 1:
                chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_vol()->set_volume(0);
                break;
            case 2: // 音量 50%
                chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_vol()->set_volume(50);
                break;
            case 3: // 音量 100%
                chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_vol()->set_volume(100);
                break;
            case 11: // 中文
                chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("chinese");
                break;
            case 12: // 英文
                chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("english");
                break;
            case 13: // 日语
                chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("xiaoriben");
                break;
            case 14: // 韩语
                chassis_ctrl_->mutable_misc_ctrl()->mutable_audio()->mutable_pack()->set_dft_pack("korean");
                break;
            default:
                break;
        }
    }

    void KeySimulate::CameraSetting(const int val) {
#if 0
        switch (val)
        {
            case 0:
                // capture a frame save to file
                chassis_ctrl_->mutable_misc_ctrl()->mutable_camera()->set_capture(true);
                break;
            case 1:
                //TODO: record images as a video
                //chassis_ctrl_->mutable_misc_ctrl()->mutable_camera()->set_video(true);
                break;
            case 2:
                // close all cameras
                chassis_ctrl_->mutable_misc_ctrl()->mutable_camera()->set_close(true);
                break;
            case 3:
                // restart all cameras
                chassis_ctrl_->mutable_misc_ctrl()->mutable_camera()->set_restart(true);
                break;
            default:
                break;
        }
#endif
    }

    void KeySimulate::MissionSetting(const int val) {
        switch (val) {
            case 0: {
                AINFO << "mission planning!";
                mission_setting_->set_cmd(E_CTRL_TYPE_START);
                mission_setting_->set_key(E_KEY_POWER);
                mission_setting_->set_user(E_CMD_FROM_SIMKEY);
                mission_setting_->set_mode(E_CLEAN_GLOBAL);
                auto x = mission_setting_->mutable_mzp()->add_szp();
                x->set_index(323);
                x->set_name("airport");
                x->set_area_typs(E_AREA_TYPE_ALL);
                x->mutable_clean_parm()->set_cnt(E_HDL_TWICE);
                x->mutable_clean_parm()->set_attr(E_JOB_ATTR_MS);
                x->mutable_clean_parm()->set_fan_strength(4);
                x->mutable_clean_parm()->set_water_flow(2);
                x->mutable_clean_parm()->set_water_scrape(1);
                x->mutable_clean_parm()->set_left_disc_brush(10);
                x->mutable_clean_parm()->set_right_disc_brush(11);
                x->mutable_clean_parm()->set_unique_id(312);
                AINFO << "mission planning OK!";
                break;
            }
            case 1:
                AINFO << "TODO";
                break;
            default:
                AWARN << "not support cmd: " << val;
                break;
        }
    }

} // namespace device
} // namespace camb
} // namespace brain

#include "modules/chassis/parser/pack_libs/raw_packer.h"

namespace mstf {
namespace chss {
namespace parser {

    InfoPacker::InfoPacker() {
        AINFO << "InfoPacker construct";
    }

    InfoPacker::~InfoPacker() {
        AINFO << "InfoPacker de-construct";
    }

    std::vector<uint8_t> InfoPacker::PackMessage(const
            DownToMcuData& to_uart) {
        uint16_t size = 0;
        uint16_t check_bit = 0;
        std::vector<uint8_t> d_pack;

        if (this->frame_seq_ >= UINT16_MAX)
            this->frame_seq_ = 1;
        else
            this->frame_seq_++;

        // head
        d_pack.push_back(E_MCU_FRAME_HEAD_1 & 0xff);
        d_pack.push_back(E_MCU_FRAME_HEAD_2 & 0xff);

        // length
        d_pack.push_back(0);
        d_pack.push_back(0);

        // sequence
        d_pack.push_back(static_cast<uint8_t>((frame_seq_ >> 8) & 0xff));
        d_pack.push_back(static_cast<uint8_t>(frame_seq_ & 0xff));

        // command type
        d_pack.push_back(static_cast<uint8_t>((to_uart.e_command() >> 8) & 0xff));
        d_pack.push_back(static_cast<uint8_t>(to_uart.e_command() & 0xff));

        // insert specific data
        if (this->InsertData(to_uart, d_pack) == -1) {
            AFATAL << "wrong data";
            std::vector<uint8_t>().swap(d_pack);
            return d_pack;
        }

        // specific length
        size = d_pack.size() - (E_MCU_FRAME_HT_LEN & 0xff) +
            (E_MCU_FRAME_CRC_SIZE & 0xff);
        d_pack[2] = ((size >> 8) & 0xFF);
        d_pack[3] = (size & 0xFF);

        // checksum
        check_bit = GenerateCheckSum((uint8_t*)&d_pack[2], d_pack.size() - 2);
        d_pack.push_back(static_cast<uint8_t>((check_bit >> 8) & 0xFF));
        d_pack.push_back(static_cast<uint8_t>(check_bit & 0xFF));

        // tail
        d_pack.push_back(E_MCU_FRAME_TAIL_1 & 0xff);
        d_pack.push_back(E_MCU_FRAME_TAIL_2 & 0xff);


#ifdef CHSS_PKG_DBG
        std::ostringstream os;
        os << "[";
        for (auto i : d_pack) {
            os << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<int>(i) << " ";
        }
        os << "]";
        AINFO << "send mcu contents: " << os.str();
#endif
        return d_pack;
    }

    uint16_t InfoPacker::InsertData(const
            proto::DownToMcuData& proto_raw_data,
            std::vector<uint8_t>& frame_data_pack) {
        switch (proto_raw_data.e_command()) {
        case E_SERIAL_GETONLINEINFO: {  //发送联机信号
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialgetOnLineInfo";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.online_msg().dev_type() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.online_msg().proto_version() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.online_msg().proto_version() & 0xFF));
            break;
        }
        case E_SERIAL_GETHEARTBEATINFO: {  //发送心跳命令
#ifdef CHSS_PKG_DBG
            AINFO << "get heartbeat info";
#endif
            break;
        }
        case E_SERIAL_GETSLAVEDEVICEINFO: {  //发送获取底盘信息
#ifdef CHSS_PKG_DBG
            AINFO << "get slave device info";
#endif
            break;
        }
        case E_SERIAL_GETSENSORENABLEINFO: {  //发送获取传感器的开关状态
#ifdef CHSS_PKG_DBG
            AINFO << "get sensor enable info";
#endif
            break;
        }
        case E_SERIAL_GETSENSORVALINFO: {  //发送获取传感器的值
#ifdef CHSS_PKG_DBG
            AINFO << "get sensor value info";
#endif
            break;
        }
        case E_SERIAL_SETSENSORENABLE: {  //设置传感器开关状态
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetSensorEnable";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_sensor_enable().sensor_status() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_sensor_enable().sensor_status() & 0xFF));
            break;
        }
        case E_SERIAL_SETROBOTVELOCITY: {  //设置机器人速度
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetRobotVelocity";
#endif
#ifdef SET_ROBOT_LRW
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_wheels_speed().lw_speed() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_wheels_speed().lw_speed() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_wheels_speed().rw_speed() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_wheels_speed().rw_speed() & 0xFF));
#else
            /*
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_wheels_speed().linear_velocity() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_wheels_speed().linear_velocity() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_wheels_speed().palstance() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_wheels_speed().palstance() & 0xFF));
                        */
#endif
            break;
        }
        case E_SERIAL_SETWIFILEDDISPLAY: {  //设置wifi灯显
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetWifiLedDisplay";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_wifi_led_display().display_type() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_wifi_led_display().flash_times() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_wifi_led_display().flash_times() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_wifi_led_display().flash_rate() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_wifi_led_display().flashed_state() & 0xFF));
            break;
        }
        case E_SERIAL_SETFANMOTORVELOCITY: {  //设置风机吸力
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetFanMotorVelocity";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_fan_suction().fan_motor_level() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_fan_suction().fan_motor_level() & 0xFF));
            break;
        }
        case E_SERIAL_SETSENSORCALIBRATE: {  //设置传感器标定
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetSensorCalibrate";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_sensor_calibration().wall_sensor_state() & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_sensor_calibration().wall_sensor_param() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_sensor_calibration().floor_sensor_state() & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_sensor_calibration().floor_sensor_param() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_sensor_calibration().imu_state() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_sensor_calibration().tof_state() & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_sensor_calibration().tof_param() & 0xFF));
            break;
        }
        case E_SERIAL_SETTIMERREPORTPARAM: {  //设置定时传输的参数(上传时间、是否上传)
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetTimerReportParam";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.mcu_sensor_conf().bit_mask_0() & 0xFF));

            if (proto_raw_data.mcu_sensor_conf().bit_mask_0() & 0x80) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().bit_mask_1() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_0() & (1 << 0)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().tof_data_freq() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_0() & (1 << 1)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().whl_current_freq() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_0() & (1 << 2)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().dock_sig_freq() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_0() & (1 << 3)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().imu_data_freq() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_0() & (1 << 4)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().euler_freq() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_0() & (1 << 5)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().whl_pulse_freq() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_0() & (1 << 6)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().pose_data_freq() & 0xFF));
            }
            if (proto_raw_data.mcu_sensor_conf().bit_mask_1() & (1 << 0)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().slave_time_freq() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_1() & (1 << 1)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().ifr_wall_freq() & 0xFF));
            }

            if (proto_raw_data.mcu_sensor_conf().bit_mask_1() & (1 << 2)) {
                frame_data_pack.push_back(static_cast<uint8_t>(
                            proto_raw_data.mcu_sensor_conf().whl_speed_freq() & 0xFF));
            }
            break;
        }
        case E_SERIAL_SETROBOTPWROFF: {  //关机命令
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetRobotPwrOff";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(proto_raw_data.set_power().option() & 0xFF));
            break;
        }
        case E_SERIAL_SETUILEDDISPLAY: {  //设置UI灯显
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetUiLedDisplay";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().clean_display_type() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_clean_display().clean_breathe_interval() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().clean_breathe_interval() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().clean_flash_rate() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().clean_flashed_state() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().charge_display_type() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_clean_display().charge_breathe_interval() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().charge_breathe_interval() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().charge_flash_rate() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().charge_flashed_state() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().error_display_type() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_clean_display().error_breathe_interval() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().error_breathe_interval() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().error_flash_rate() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_clean_display().error_flashed_state() & 0xFF));
            break;
        }
        case E_SERIAL_SETMAINMOTORVELOCITY: {  //设置中扫速度
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetMainMotorVelocity";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_main_brush_speed().main_motor_level() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_main_brush_speed().main_motor_level() & 0xFF));
            break;
        }
        case E_SERIAL_SETSIDEMOTORVELOCITY: {  //设置变扫速度
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetSideMotorVelocity";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_side_brush_speed().side_motor_level() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_side_brush_speed().side_motor_level() & 0xFF));
            break;
        }
        case E_SERIAL_SETRESETSENSER: {  //复位传感器
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialsetResetSenser";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_reset_sensor().reset() & 0xFF));
            break;
        }
        case E_SERIAL_PUMPMOTORVELOCITY: {  //水泵传感器
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialPumpMotorVelocity";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_pump_speed().pump_motor_level() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_pump_speed().pump_motor_level() & 0xFF));
            break;
        }
        case E_SERIAL_SETDISHCLOTHMOTORVELOCITY: {  //抹布转盘
#ifdef CHSS_PKG_DBG
            AINFO << "E_serialsetDishclothMotorVelocity";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.set_turnable_speed().
                         mop_motor_level() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.set_turnable_speed().
                        mop_motor_level() & 0xFF));
            break;
        }

        case E_SERIAL_FLIP_CAMERA: {
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.flip_camera().flip() & 0xFF));
        }
            break;
        case E_SERIAL_MCUUPGRADESTART: {  // MCU升级开始
#ifdef CHSS_PKG_DBG
            AINFO << "MCU start update";
#endif
            break;
        }
        case E_SERIAL_MCUUPGRADESTATUS: {  // MCU升级状态
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialMcuUpgradeStatus";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.update_status().result() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.update_status().result() & 0xFF));
            break;
        }
        case E_SERIAL_MCUUPGRADEDATA: {  //升级数据
#ifdef CHSS_PKG_DBG
            AINFO << "E_SerialMcuUpgradeData";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.update_data_frame().data_seq() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.update_data_frame().data_seq() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.update_data_frame().data_len() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.update_data_frame().data_len() & 0xFF));

            frame_data_pack.insert(frame_data_pack.end(),
                    proto_raw_data.update_data_frame().updata_data().begin(),
                    proto_raw_data.update_data_frame().updata_data().end());
            break;
        }
        case E_SERIAL_SENDACKTOMCU: {    /* 接收确认报文 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_SENDACKTOMCU";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.ack_mcu().recv_result() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.ack_mcu().upstream_cmd() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.ack_mcu().upstream_cmd() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_ENABLEFACTORY: { /* 切换工厂测试模式 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_ENABLEFACTORY";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_enable_factory().factory_status() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_MCUDEVICEINFO: { /* 获取系统信息 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_MCUDEVICEINFO";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_get_mcu_device_info().get_mcu_device_info() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_HEARTBEAT: { /* 心跳报文 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_HEARTBEAT";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_heartbeat().get_heartbeat() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_FUNCTIONCONTROL: { /* 数显开关报文 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_FUNCTIONCONTROL";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_digital_display().enable_digital_display() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_MOTORS: { /* 电机开关报文 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_MOTORS";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_motors().enable_motors() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_SETCLIFFDATA: { /* 地检标定报文 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_SETCLIFFDATA";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.factory_sensor_calibration().cliff_calibration_value() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_sensor_calibration().cliff_calibration_value() &0xFF));
            break;
        }
        case E_SERIAL_FACTORY_SETWALLSENSORDATA: { /* 墙检标定报文 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_SETWALLSENSORDATA";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.factory_sensor_calibration().wall_calibration_value() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_sensor_calibration().wall_calibration_value() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_SETIMUDATA: { /* imu标定报文 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_SETIMUDATA";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.factory_sensor_calibration().imu_calibration_value() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_sensor_calibration().imu_calibration_value() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_SETTOFDATA: { /* tof标定报文 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_SETTOFDATA";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.factory_sensor_calibration().tof_calibration_value() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_sensor_calibration().tof_calibration_value() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_WIFI: { /* wifi测试 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_WIFI";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(proto_raw_data.factory_wifiset().wifi_set() & 0xFF));

            frame_data_pack.insert(frame_data_pack.end(),
                    proto_raw_data.factory_wifiset().wifi_name().begin(),
                    proto_raw_data.factory_wifiset().wifi_name().end());

            frame_data_pack.insert(frame_data_pack.end(),
                    proto_raw_data.factory_wifiset().wifi_pwd().begin(),
                    proto_raw_data.factory_wifiset().wifi_pwd().end());
            break;
        }
        case E_SERIAL_FACTORY_LIDARINFO: { /* 雷达测试 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_LIDARINFO";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_lidartest().lidar_status() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.factory_lidartest().lidar_start_angle() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_lidartest().lidar_start_angle() & 0xFF));

            frame_data_pack.push_back(static_cast<uint8_t>(
                        (proto_raw_data.factory_lidartest().lidar_end_angle() >> 8) & 0xFF));
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_lidartest().lidar_end_angle() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_ALGODEVICEINFO: { /* 获取算法板信息 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_ALGODEVICEINFO";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_algodevice_info().get_algo_device_info() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_DELETELOCALCONFIG: { /* 删除本地配置文件 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_DELETELOCALCONFIG";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_delete_configfile().delete_config_file() & 0xFF));
            break;
        }
        case E_SERIAL_FACTORY_WRITESN: { /* 写入SN,MAC,KEY,UUID等 */
#ifdef CHSS_PKG_DBG
            AINFO << "E_SERIAL_FACTORY_WRITESN";
#endif
            frame_data_pack.push_back(static_cast<uint8_t>(
                        proto_raw_data.factory_write_baseinfo().baseinfo_cmd() & 0xFF));

            frame_data_pack.insert(frame_data_pack.end(),
                    proto_raw_data.factory_write_baseinfo().baseinfo_sn().begin(),
                    proto_raw_data.factory_write_baseinfo().baseinfo_sn().end());

            frame_data_pack.insert(frame_data_pack.end(),
                    proto_raw_data.factory_write_baseinfo().baseinfo_mac().begin(),
                    proto_raw_data.factory_write_baseinfo().baseinfo_mac().end());

            frame_data_pack.insert(frame_data_pack.end(),
                    proto_raw_data.factory_write_baseinfo().baseinfo_key().begin(),
                    proto_raw_data.factory_write_baseinfo().baseinfo_key().end());

            frame_data_pack.insert(frame_data_pack.end(),
                    proto_raw_data.factory_write_baseinfo().baseinfo_uuid().begin(),
                    proto_raw_data.factory_write_baseinfo().baseinfo_uuid().end());
            break;
        }
        default: {
            AFATAL << "wrong cmd";
            return -1;
        }
        }
        return 0;
    }

    std::vector<uint8_t> InfoPacker::PackMotorMessage(const
            DownToMcuData& to_servo) {
        std::vector<uint8_t> d_pack;

        d_pack.push_back('6');
        d_pack.push_back('0');
        d_pack.push_back('1');

        d_pack.push_back('#');

        d_pack.push_back('e');
        d_pack.push_back('9');
        d_pack.push_back('0');
        d_pack.push_back('0');
        d_pack.push_back('2');
        d_pack.push_back('1');
        d_pack.push_back('0');
        d_pack.push_back('0');

        if (to_servo.set_wheels_speed().linear() == 0 &&
                to_servo.set_wheels_speed().angular() == 0) {
            //stop
            d_pack.push_back('0');
            d_pack.push_back('0');
            d_pack.push_back('0');
            d_pack.push_back('0');
            d_pack.push_back('0');
            d_pack.push_back('0');
            d_pack.push_back('0');
            d_pack.push_back('0');
        } else {
            d_pack.push_back('0');
            d_pack.push_back('1');
            d_pack.push_back('0');
            d_pack.push_back('0');
            d_pack.push_back('0');
            d_pack.push_back('1');
            d_pack.push_back('0');
            d_pack.push_back('0');
        }

        return d_pack;
    }

    uint16_t InfoPacker::GenerateCheckSum(uint8_t* puchMsg, uint32_t usDataLen) {
        unsigned short wCRCin = 0xFFFF;  //初值 0xFFFF
        unsigned short wCPoly = 0x1021;  //多项式 x16+x12+x5+1
        unsigned char wChar = 0;
        while (usDataLen--) {
            wChar = *(puchMsg++);
            wCRCin ^= (wChar << 8);
            for (int i = 0; i < 8; i++) {
                if (wCRCin & 0x8000) {
                    wCRCin = (wCRCin << 1) ^ wCPoly;
                } else {
                    wCRCin = wCRCin << 1;
                }
            }
        }
        return wCRCin;
    }

} //namespace parser
} //namespace chss
} //namespace mstf

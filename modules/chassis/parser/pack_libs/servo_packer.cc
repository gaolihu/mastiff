#include <cmath>

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

    ServoPacker::ServoPacker(const
            std::string& dev) :
        PackerBaseItf(dev) {
#ifdef CHSS_PKG_DBG
            AINFO << "ServoPacker construct for " << dev;
#endif
        }

    ServoPacker::~ServoPacker() {
#ifdef CHSS_PKG_DBG
        AINFO << "ServoPacker de-construct";
#endif
    }

    std::vector<std::tuple<const int,
        const std::vector<uint8_t>>>
    ServoPacker::PackServoMtrControl(const
            ChsMovementCtrl& ctrl) {
        //AINFO << "packing servo control";
        //convert servo control commands
        std::vector<std::tuple<const int,
            const std::vector<uint8_t>>> tds;

        if (ctrl.has_diff_spd()) {
            //diff speed -> RPM
            WheelRpm rpm;
            ConvertDiffSpeed2Rpm(ctrl.diff_spd(), rpm);
            FillDataContainerCanOpenAxleSpeed(tds,
                    rpm.left_rpm() * 10,
                    rpm.right_rpm() * 10);
            return tds;
        } else if (ctrl.has_whl_spd()) {
            //wheel speed -> RPM
            WheelRpm rpm;
            ConvertWheelSpeed2Rpm(ctrl.whl_spd(), rpm);
            FillDataContainerCanOpenAxleSpeed(tds,
                    rpm.left_rpm() * 10,
                    rpm.right_rpm() * 10);
            return tds;
        } else if (ctrl.has_whl_rpm()) {
            //-> RPM
            FillDataContainerCanOpenAxleSpeed(tds,
                    ctrl.whl_rpm().left_rpm() * 10,
                    ctrl.whl_rpm().right_rpm() * 10);
            return tds;
        }

        //other commands
        //TODO
        if (ctrl.has_emergency_stop()) {
        } else if (ctrl.has_wheel_reverse()) {
        } else if (ctrl.has_wheel_release()) {
        } else if (ctrl.has_encoder_reset()) {
        } else if (ctrl.has_update_freq_ms()) {
        }

        return {};
    }

    //FREE CAN for servo control
    std::vector<std::tuple<const int,
        const std::vector<uint8_t>>>
    ServoPacker::PackServoMtrSetting(const
            ServoSetting& ss) {
        uint8_t cmd[200] = {0};
        int16_t addr[200] = {0};
        uint8_t sub_id[200] = {0};
        int value[200] = {0};
        int size = 0;

        std::vector<std::tuple<const int,
            const std::vector<uint8_t>>> tds;

        switch (ss.dev_manage().sub_opts()) {
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
                FillDataContainerCanOpenAxleSpeed(tds, 0, 0);
                FillDataContainerCanOpenAxleDisable(tds);
                FillDataContainerCanOpenResetStatus(tds);
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
                AERROR << "option TODO for " << ss.dev_manage().sub_opts();
        }

        return tds;
    }

    //CANOPEN speed control
#if 0
    std::vector<std::tuple<const int, const std::vector<uint8_t>>>
    ServoPacker::PackMotorDiffSpeedDouble(const ChsMovementCtrl& sd) {
        std::vector<std::tuple<const int,
            const std::vector<uint8_t>>> tds;
#if 1
#if 0
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
#endif
        return {};
    }
#endif

    //FREE CAN protocol for diff speed control
    std::tuple<const int, const std::vector<uint8_t>>
    ServoPacker::PackMotorDiffSpeed(const WheelDiffSpd& df) {
        int size = 0;
        int16_t addr[200] = {0};
        int value[200] = {0};
        std::vector<uint8_t> data;
#if 0
        AINFO << "packing diff speed, " <<
            "linear: " << df.linear() <<
            "m/s, angular: " << df.angular() <<
            "rad/s";
#endif
        config_servo_motor_wheel_speed(addr, value, &size);

        data.emplace_back(DS_MOTOR_WRITE_DIFF_SPEED);
        data.emplace_back(0x00);
        data.emplace_back(0x00);
        data.emplace_back(0x00);

        //line speed
        data.emplace_back((int)(df.linear() * 1000) & 0xff);
        data.emplace_back(((int)(df.linear() * 1000) & 0xff00) >> 8);

        //angulor speed
        data.emplace_back((int)(df.angular() * 1000) & 0xff);
        data.emplace_back(((int)(df.angular() * 1000) & 0xff00) >> 8);

        return {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data};
    }

    //FREE CAN for wheel reverse
    std::tuple<const int,
        const std::vector<uint8_t>>
    PackMotorWheelReverse(const bool r) {
        int size = 0;
        int16_t addr[200] = {0};
        int value[200] = {0};
        std::vector<uint8_t> data;
        config_servo_motor_wheel_reverse(addr, value, &size);

        data.emplace_back(DS_MOTOR_WRITE_BOTH_CMD);
        data.emplace_back(addr[0] & 0xff);
        data.emplace_back(((addr[0]) & 0xff00) >> 8);
        data.emplace_back(0x00);

        data.emplace_back(r);
        data.emplace_back(0x00);
        data.emplace_back(0x00);
        data.emplace_back(0x00);

        return {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data};
    }

    //FREE CAN for wheel speed control
    std::tuple<const int,
        const std::vector<uint8_t>>
    ServoPacker::PackMotorWheelSpeed(const
            WheelDiffSpd& df) {
        int size = 0;
        int16_t addr[200] = {0};
        int value[200] = {0};
        std::vector<uint8_t> data;

        WheelRpm rpm;
        ConvertDiffSpeed2Rpm(df, rpm);
#if 0
        AINFO << "packing wheel speed, RPM" <<
            " left :" << std::hex << rpm.left_rpm() <<
            ", right :" << std::hex << rpm.right_rpm();
#endif
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

    ///////////// Tesing /////////////
    const std::vector<uint8_t>
    ServoPacker::PackServoMtrRaw(const
            WheelDiffSpd& df) {
        std::vector<uint8_t> servo_str;

            WheelRpm rpm;
            ConvertDiffSpeed2Rpm(df, rpm);
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
        return servo_str;
    }

    void ServoPacker::ConvertDiffSpeed2Wheel(const
            WheelDiffSpd& ds, WheelSpeed& ws) {
        ws.set_left_whl((ds.linear() -
                    WHEEL_DISTANCE *
                    ds.angular() / 2));
        ws.set_right_whl((ds.linear() +
                    WHEEL_DISTANCE *
                    ds.angular() / 2));
    }

    void ServoPacker::ConvertDiffSpeed2Rpm(const
            WheelDiffSpd& ds, WheelRpm& wr) {
        float vl = (ds.linear() - WHEEL_DISTANCE *
                ds.angular() / 2);
        float vr = (ds.linear() + WHEEL_DISTANCE *
                ds.angular() / 2);
        wr.set_left_rpm((int)60 * vl /
                (M_PI * WHEEL_DIAMETER));
        wr.set_right_rpm((int)60 * vr /
                (M_PI * WHEEL_DIAMETER));
    }

    void ServoPacker::ConvertWheelSpeed2Rpm(const
            WheelSpeed& ws, WheelRpm& wr) {
        wr.set_left_rpm((int)60 * ws.left_whl() /
                (M_PI * WHEEL_DIAMETER));
        wr.set_right_rpm((int)60 * ws.right_whl() /
                (M_PI * WHEEL_DIAMETER));
    }

    bool ServoPacker::FillDataContainerFreeRead(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td,
            const std::string& des,
            const int16_t* addr,
            const int* value,
            const int size) {
        std::vector<uint8_t> data;

        if (addr == nullptr || size <= 0) {
            AERROR << "fill motor data adress / size error";
            return false;
        }

        for (int i = 0; i < size; i++) {
            data.clear();
            data.emplace_back(DS_MOTOR_READ_BOTH_CMD);
            data.emplace_back(addr[i] & 0xff);
            data.emplace_back((addr[i] & 0xff00) >> 8);
            data.emplace_back(0x00);

            data.emplace_back(value[i] & 0xff);
            data.emplace_back((value[i] & 0xff00) >> 8);
            data.emplace_back((value[i] & 0xff0000) >> 16);
            data.emplace_back((value[i] & 0xff000000) >> 24);
#if 0
            AWARN << "get servo double read " << des <<
                "-" << std::setw(3) << std::setfill('0') << i <<
                ", addr: " << std::hex << std::setw(4) << std::setfill('0') << addr[i] <<
                "[ " << std::setw(2) << std::setfill('0') << static_cast<int>(data[0]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[1]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[2]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[3]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[4]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[5]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[6]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[7]) <<
                " ]" << ", write value: " << std::hex << value[i];
#endif
            td.emplace_back<std::tuple<const int,
                const std::vector<uint8_t>>>(
                        {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data});
        }

        return true;
    }

    //double write registers
    bool ServoPacker::FillDataContainerFreeWrite(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td,
            const std::string& des,
            const int16_t* addr,
            const int* value,
            const int size) {
        std::vector<uint8_t> data;

        if (addr == nullptr || size <= 0) {
            AERROR << "fill motor data adress / size error";
            return false;
        }

        for (int i = 0; i < size; i++) {
            data.clear();
            data.emplace_back(DS_MOTOR_WRITE_BOTH_CMD);
            data.emplace_back(addr[i] & 0xff);
            data.emplace_back((addr[i] & 0xff00) >> 8);
            data.emplace_back(0x00);

            data.emplace_back((value[i] & 0xff0000) >> 16);
            data.emplace_back((value[i] & 0xff000000) >> 24);
            data.emplace_back(value[i] & 0xff);
            data.emplace_back((value[i] & 0xff00) >> 8);
#if 1
            AWARN << "get servo double write " << des <<
                "-" << std::setw(3) << std::setfill('0') << i <<
                ", addr: " << std::hex << std::setw(4) << std::setfill('0') << addr[i] <<
                "[ " << std::setw(2) << std::setfill('0') << static_cast<int>(data[0]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[1]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[2]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[3]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[4]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[5]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[6]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[7]) <<
                " ]" << ", write value: " << std::hex << value[i];
#endif
            td.emplace_back<std::tuple<const int,
                const std::vector<uint8_t>>>(
                        {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data});
        }

        return true;
    }

    bool ServoPacker::FillDataContainerCanOpenConfig(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td,
            const uint8_t* cmd,
            const int16_t* addr,
            const uint8_t* sub_id,
            const int* value,
            const int size) {
        std::vector<uint8_t> data;

        if (addr == nullptr || size <= 0) {
            AERROR << "fill motor data adress / size error";
            return false;
        }

        for (int i = 0; i < size; i++) {
            data.clear();

            data.emplace_back(cmd[i]);
            data.emplace_back(addr[i] & 0xff);
            data.emplace_back((addr[i] & 0xff00) >> 8);
            data.emplace_back(sub_id[i]);

            data.emplace_back((value[i] & 0xff0000) >> 16);
            data.emplace_back((value[i] & 0xff000000) >> 24);
            data.emplace_back(value[i] & 0xff);
            data.emplace_back((value[i] & 0xff00) >> 8);

            td.emplace_back<std::tuple<const int,
                const std::vector<uint8_t>>>(
                        {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data});
            td.emplace_back<std::tuple<const int,
                const std::vector<uint8_t>>>(
                        {DS_MOTOR_DRIVE_MOTOR_AXLE_2, data});
#if 1
            AWARN << "get servo can config " <<
                " " << std::setw(3) << std::setfill('0') << i <<
                ", addr: " << std::hex << std::setw(4) << std::setfill('0') << addr[i] <<
                "[ " << std::setw(2) << std::setfill('0') << static_cast<int>(data[0]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[1]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[2]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[3]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[4]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[5]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[6]) <<
                " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[7]) <<
                " ]" << ", write value: " << std::hex << value[i];
#endif
        }

        return true;
    }

    void ServoPacker::FillDataContainerCanOpenActivate(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td) {
        //activate axle
        td.emplace_back<std::tuple<const int,
        const std::vector<uint8_t>>>(
                {0, {01, 01}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {0, {01, 02}});
        AWARN << "config activate motor " <<
            " id 0x00, data[0x01, 0x01 / 0x01, 0x02]";
    }

    void ServoPacker::FillDataContainerCanOpenStartReporting(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td) {
        //stop reporting data
        //speed & encoder
        td.emplace_back<std::tuple<const int,
        const std::vector<uint8_t>>>(
                {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                {0x2f, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x2f, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00}});
        //status
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                    {0x2f, 0x01, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x2f, 0x01, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00}});

        AWARN << "config start reporting data" <<
            " id 0x00, data[0x02, 0x01 / 0x02, 0x02]";
    }

    void ServoPacker::FillDataContainerCanOpenStopReporting(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td) {
        //start reporting data
        td.emplace_back<std::tuple<const int,
        const std::vector<uint8_t>>>(
                {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                {0x2f, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x2f, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00}});
        //status
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                    {0x2f, 0x01, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x2f, 0x01, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00}});
        AWARN << "config stop reporting data" <<
            " id 0x00, data[0x02, 0x01 / 0x02, 0x02]";
    }

    void ServoPacker::FillDataContainerCanOpenSync(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td) {
        //sync
        td.emplace_back<std::tuple<const int,
        const std::vector<uint8_t>>>(
                {0x80, {}});
        AWARN << "config sync canOpen" <<
            " id 0x80, data[]";
    }

    void ServoPacker::FillDataContainerCanOpenResetStatus(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td) {
        //reset speed
        td.emplace_back<std::tuple<const int,
        const std::vector<uint8_t>>>(
                {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                {0x23, 0x69, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x23, 0x69, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
        //reset encoder
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                    {0x23, 0x63, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x23, 0x63, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
        AWARN << "config canOpen reset speed & encoder";
    }

    void ServoPacker::FillDataContainerCanOpenAxleSpeed(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td,
            const int32_t left,
            const int32_t right) {
        //setting canopen speed
        td.emplace_back<std::tuple<const int,
        const std::vector<uint8_t>>>(
#if 1
                {0x401, {
#else
                {DS_MOTOR_DRIVE_MOTOR_AXLE_1, {0x23, 0xff, 0x60, 0x00,
#endif
                (uint8_t)((~right + 1) & 0xff),
                (uint8_t)(((~right + 1) & 0xff00) >> 8),
                (uint8_t)(((~right + 1) & 0xff0000) >> 16),
                (uint8_t)(((~right + 1) & 0xff000000) >> 24)}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
#if 1
                {0x402, {
#else
                {DS_MOTOR_DRIVE_MOTOR_AXLE_2, {0x23, 0xff, 0x60, 0x00,
#endif
                (uint8_t)(left & 0xff),
                (uint8_t)((left & 0xff00) >> 8),
                (uint8_t)((left & 0xff0000) >> 16),
                (uint8_t)((left & 0xff000000) >> 24)}});
#if 0
            //read speed
            td.emplace_back<std::tuple<const int,
                const std::vector<uint8_t>>>(
                        {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                        {0x43, 0x6b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
            td.emplace_back<std::tuple<const int,
                const std::vector<uint8_t>>>(
                        {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                        {0x43, 0x6b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
#endif
            /*
               AWARN << "canOpen speed setting left: " << left <<
               ", right: " << right;
               */
    }

    void ServoPacker::FillDataContainerCanOpenAxleEnable(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td) {
        //axle enable
        td.emplace_back<std::tuple<const int,
        const std::vector<uint8_t>>>(
                {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                {0x2B, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x2B, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00}});
        AWARN << "motor axle enable" <<
            " id " << std::hex << DS_MOTOR_DRIVE_MOTOR_AXLE_1 <<
            " / " << std::hex << DS_MOTOR_DRIVE_MOTOR_AXLE_2 <<
            ", data[0x2B, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00]";
    }

    void ServoPacker::FillDataContainerCanOpenAxleDisable(
            std::vector<std::tuple<const int,
            const std::vector<uint8_t>>>& td) {
        //axle disable
        td.emplace_back<std::tuple<const int,
        const std::vector<uint8_t>>>(
                {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                {0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                    {0x2B, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00}});
        td.emplace_back<std::tuple<const int,
            const std::vector<uint8_t>>>(
                    {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                    {0x2B, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00}});
        AWARN << "motor axle disable" <<
            " id " << std::hex << DS_MOTOR_DRIVE_MOTOR_AXLE_1 <<
            " / " << std::hex << DS_MOTOR_DRIVE_MOTOR_AXLE_2 <<
            ", data[0x2B, 0x40, 0x60, 0x00, 0x06/0x07, 0x00, 0x00, 0x00]";

    }

} //namespace parser
} //namespace chss
} //namespace mstf

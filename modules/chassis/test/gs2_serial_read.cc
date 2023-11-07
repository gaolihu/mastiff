/*
 * @Date: 2023-10-13 18:10:17
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/test/gs2_serial_read.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
#include "cyber/cyber.h"
#include "cyber/time/clock.h"
#include "modules/chassis/drivers/uart/serial_data.h"
#include "modules/chassis/proto/chassis_config.pb.h"
using namespace std::placeholders;

using namespace apollo;
using namespace mstf;
using namespace chss;

void callback(const uint8_t *, const size_t) {
    AWARN << "got data!" ;
}

const static std::string g_dev_name = "/dev/ttyS0";
int                      main(int argc, char const *argv[]) {
    AINFO << "chassis start, name: ";

    proto::UartConfig config;
    config.set_dev_name(g_dev_name);

    proto::UartDevSetting     setting;
    proto::CircularBufSetting buffer;
    setting.set_uart_dev(g_dev_name);
    setting.set_baud(921600);
    setting.set_data_bits(8);
    setting.set_stop_bits(1);
    setting.set_parity(110);
    setting.set_once_read_size(256);
    setting.set_serial_read_freq(5);

    buffer.set_parser_freq(5);
    buffer.set_circle_name("ydlidar_gs2");
    buffer.set_circlular_size(20480);

    config.mutable_dev_setting()->CopyFrom(setting);
    config.mutable_buf_setting()->CopyFrom(buffer);
    config.set_dev_name(g_dev_name);

    driver::SerialData serial(std::bind(&callback, ::_1, ::_2), &config);
    serial.Init(0, nullptr);
    serial.Start();

    //exit handler
    auto chassis_exit_hdl = [&](void) {
        AWARN << "get exit signal, shutdown!";

        // ctrl_receiver_.reset();
        // chas_dispatcher_.reset();
        // chs_conf_.reset();

        AINFO << "chassis shutdown finish!";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };

    ::cyber::RegisterUserExitHandle(chassis_exit_hdl);

    AINFO << "Cyber Chassis Wait\n...";

    ::cyber::WaitForShutdown();
    serial.Close();
    return 0;
}

/*
 * @Date: 2023-09-14 11:45:23
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/test/wifi_tool_test.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "../drivers/soc/network/wifi_tool.h"

#include "../drivers/soc/network/utils.h"
#include "../drivers/soc/network/wifi_sta.h"
#include "modules/chassis/dispatch_chs.h"
#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/receive_ctrl.h"

using namespace apollo;
using namespace mstf;
using namespace chss;

static network::WiFiTool tool;

static std::unique_ptr<DispatchChs>            g_chas_dispatcher;
static std::unique_ptr<ReceiveCtrl>            g_ctrl_receiver;
static std::shared_ptr<::proto::ChassisConfig> g_chs_conf;

static const std::string pkg_name = "chassis";

void TestRun() {
    std::vector<std::string> out;
    network::Run("cat /proc/modules", out);
    for (auto &l : out) {
        std::cout << l << std::endl;
    }
}
void GetPid(const std::string &name) {
    auto pids = network::GetProcessPid(name);
    for (auto &p : pids) {
        std::cout << p << std::endl;
    }
}
void GetMode() {
    network::Mode m = tool.GetCurrentMode();
    tool.Mode2Str(m);
}
void BasicInfo() {
    std::cout << "ip: " << tool.GetStaIp() << std::endl;
    std::cout << "mac: " << tool.GetStaMac() << std::endl;
    std::cout << "dns: " << tool.GetDns() << std::endl;
}
void CheckConnection() {
    tool.CheckConnection();
}
void CalcAvgTime() {
    tool.GetAverageTime();
}
void CommonTest() {
    std::string ssid = "😭😡😂😡❤🌚❤🐶";
    std::cout << ssid << std::endl;
    std::cout << ssid.length() << std::endl;

    std::cout << tool.help() << std::endl;
    std::cout << tool.version() << std::endl;
    std::cout << tool.Status() << std::endl;
    std::cout << tool.List() << std::endl;

    tool.Reconnect();
}
int test_wifi(int argc, char const *argv[]) {
    TestRun();
    GetPid("dhcpcd");
    GetMode();
    BasicInfo();
    CheckConnection();
    CalcAvgTime();
    CommonTest();
    return 0;
}

int main(int argc, char *argv[]) {
    AINFO << "test wifi tool" << pkg_name;
    cyber::Init(pkg_name.c_str());

    std::string chs_path = cyber::common::WorkRoot() + "/conf/ChassisConfig.conf";

    g_chs_conf = std::make_shared<::proto::ChassisConfig>();

    if (!cyber::common::GetProtoFromFile(chs_path, g_chs_conf.get())) {
        AFATAL << "get config failed";
    }
    //exit handler
    auto chassis_exit_hdl = [&](void) {
        AWARN << "get exit signal, shutdown!";

        g_ctrl_receiver.reset();
        g_chas_dispatcher.reset();
        // chs_conf_.reset();

        AINFO << "chassis shutdown finish!";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };

    g_chas_dispatcher = std::make_unique<DispatchChs>(&g_chs_conf->chs_topic_conf());
    g_ctrl_receiver   = std::make_unique<ReceiveCtrl>(g_chs_conf);

    ::cyber::RegisterUserExitHandle(chassis_exit_hdl);

    AINFO << "wifi test tool wait...";
    cyber::WaitForShutdown();
    return 0;
}

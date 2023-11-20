/*
 * @Date: 2023-11-13 15:14:07
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/network/wifi_thread.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "modules/chassis/drivers/soc/network/wifi_thread.h"

namespace mstf {
namespace chss {
namespace network {
WiFiThread::WiFiThread() {
    can_running_ = false;
}

WiFiThread::~WiFiThread() {
    if (wifi_th_.joinable())
        wifi_th_.join();
}
void WiFiThread::Init() {
    wifi_tool_ = std::make_shared<network::WiFiTool>();
}
void WiFiThread::Start() {
    can_running_ = true;

    wifi_th_ = std::thread(std::bind(&WiFiThread::WifiRunner, this));
    wifi_th_.detach();
}
void WiFiThread::Stop() {
    can_running_ = false;
    if (wifi_th_.joinable())
        wifi_th_.join();
}
void WiFiThread::Close() {
    can_running_ = false;
    if (wifi_th_.joinable())
        wifi_th_.join();
}
void WiFiThread::RegistePublisher(SocDataPublisher f) {
    soc_publisher_ = f;
}
void WiFiThread::PublishMsg(const protobuf::Message& msg) {
    if (soc_publisher_) {
        soc_publisher_(msg);
        std::string str;
        msg.SerializeToString(&str);
        ADEBUG << "publish a wifi msg: " << str;
    }
    else {
        AERROR << "no publisher to pub wifi results!";
    }
}
void WiFiThread::SetWiFiCtrl(const proto::WirelessCtrl& ctrl) {
    {
        std::lock_guard<std::mutex> lk(wifi_mtx_);
        wifi_ctrl_ = ctrl;
    }
    wifi_cv_.notify_one();
}

void WiFiThread::WifiRunner() {
    while (can_running_) {
        std::unique_lock<std::mutex> lk(wifi_mtx_);
        wifi_cv_.wait(lk);
        auto result = wifi_tool_->WifiOperate(wifi_ctrl_);
        auto response = mstf::chss::proto::WifiResponse();
        response.set_wifi_result(result);
        PublishMsg(response);
        AINFO << "wifi thread running once...";
    }
}
}  // namespace network
}  // namespace chss
}  // namespace mstf

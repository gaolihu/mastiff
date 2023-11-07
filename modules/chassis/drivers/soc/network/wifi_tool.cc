/*
 * @Date: 2023-09-13 11:29:26
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/network/wifi_tool.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "wifi_tool.h"
#include "cyber/cyber.h"

#include "utils.h"

using namespace apollo;
using namespace mstf ;
using namespace chss ;
using namespace network ;




std::string RunScripts(const std::string& args) {
    auto        cmd = WIFI_SCRIPTS + " '" + args + "'";
    std::string output;
    ::Run(cmd, output);
    return output;
}
std::string WifiTool::WifiOperate(const proto::WirelessCtrl& wifi_ctrl)
{
    AWARN << wifi_ctrl.get_wifi_info();
    std::string result="";
    switch( wifi_ctrl.get_wifi_info()){
        case ::proto::WirelessInfoType::WIFI_GET_MODE: {
            result = Mode2Str(GetCurrentMode());
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_IP:{
            result = GetStaIp();
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_MAC:{
            result = GetStaMac();
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_DNS:{
            result = GetDns();
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_WPA_INFO:{
            result = GetWpaInfo();
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_NAME:{
            result = GetWifiName();
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_ROUTER_MAC:{
            result = GetRouterMac();
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_STATUS:{
            result = Status();
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_SCAN_LIST:{
            result = List();
        }
        break;
        case ::proto::WirelessInfoType::WIFI_GET_RECONNECT:{
            Reconnect();
            //TODO: 改为异步操作
            result = "OK";
        }
        break;
        case ::proto::WirelessInfoType::WIFI_DISCONNECT:{
            Disconnect();
            result = "OK";
        }
        break;
        case ::proto::WirelessInfoType::WIFI_CHECK_CONNECTION:{
            //TODO: 改为异步操作
            if(CheckConnection()){
                result = "OK";
            }else{
                result = "Failed";
            }
        }
        break;
        case ::proto::WirelessInfoType::WIFI_TEST_SPEED:{
            //TODO: 改为异步操作
            float time_ms = GetAverageTime();
            auto result = std::to_string(time_ms);
        }
        break;
        case ::proto::WirelessInfoType::WIFI_CONNECT:{
            //TODO: 改为异步操作
            if(wifi_ctrl.wifi_ssid().empty()){
                result = "ssid incorrect";
            }else{
                ConnectWifi(wifi_ctrl.wifi_ssid(), wifi_ctrl.wifi_pswd());
                result = "OK";
            }
        }
        break;
        case ::proto::WirelessInfoType::WIFI_CREATE_AP:{
            //TODO: 改为异步操作
            if(wifi_ctrl.wifi_ssid().empty()){
                result = "ssid incorrect";
            }else{
                CreateAp(wifi_ctrl.wifi_ssid(), wifi_ctrl.wifi_pswd());
                result = "OK";
            }
        }
        break;
        default:
            result = "unknown wifi operate";
        break;
    }
    return result;
}
std::string WifiTool::help() const {
    return RunScripts("-h");
}
std::string WifiTool::version() const {
    return RunScripts("-v");
}

Mode WifiTool::GetCurrentMode() const {
    auto        cmd = WIFI_SCRIPTS + " mode";
    std::string output;
    ::Run(cmd, output);
    Mode mode = Mode::NONE;
    if (output == "managed") {
        mode = Mode::STA;
        std::cout << output << std::endl;
    }
    else if (output == "master" || output == "AP") {
        mode = Mode::AP;
        std::cout << output << std::endl;
    }
    else if (output == "monitor") {
        mode = Mode::SNIFFER;
        std::cout << output << std::endl;
    }
    else {
        std::cout << "unknown wifi mode" << std::endl;
    }
    return mode;
}
std::string WifiTool::GetStaIp() const {
    return RunScripts("ip");
}
std::string WifiTool::GetStaMac() const {
    return RunScripts("mac");
}
std::string WifiTool::GetDns() const {
    return RunScripts("dns");
}
void WifiTool::WlanUp() const {
    RunScripts("up");
}
void WifiTool::WlanDown() const {
    RunScripts("down");
}
std::string WifiTool::GetWpaInfo() const {
    return RunScripts("info");
}
std::string WifiTool::GetWifiName() const {
    return RunScripts("name");
}
std::string WifiTool::GetRouterMac() const {
    return RunScripts("bssid");
}
std::string WifiTool::ConnectWifi(const std::string& ssid, const std::string& pwd) const {
    auto ssid_copy = ssid;
    auto pwd_copy  = pwd;
    ReplaceString(ssid_copy, "'", "\\'");
    ReplaceString(pwd_copy, "'", "\\'");

    return "连接成功";
}
std::string WifiTool::Disconnect() const {
    return RunScripts("break");
}
std::string WifiTool::CreateAp(const std::string& ssid, const std::string& pwd) const {
    auto ssid_copy = ssid;
    auto pwd_copy  = pwd;
    ReplaceString(ssid_copy, "'", "\\'");
    ReplaceString(pwd_copy, "'", "\\'");
    return "开启成功";
}
std::string WifiTool::Status() const
{
    return RunScripts("status_json");
}
std::string WifiTool::List() const
{
    return RunScripts("list");
}
void WifiTool::Reconnect() const {  // reconnect
    RunScripts("reconnect");
}

//------------------------------------------
bool WifiTool::CheckConnection() const {
    auto output = RunScripts("ping");

    std::cout << "ping: " << output << std::endl;

    if ("网络连接正常" == output) {
        return true;
    }
    else {
        return false;
    }
}

float WifiTool::GetAverageTime() const {
    auto        cmd = WIFI_SCRIPTS + " speed";
    std::string output;
    ::Run(cmd, output);

    float time = 0.0f;
    if (!output.empty()) {
        time = std::atof(output.c_str());
    }
    std::cout << "wifi average time: " << time << " ms" << std::endl;

    return time;
}
std::string WifiTool::Mode2Str(const Mode& mode){
    std::string m_str;
    switch (mode){
        case Mode::NONE:
            m_str = "unknown";
        break;
        case Mode::AP:
            m_str = "ap";
        break;
        case Mode::STA:
            m_str = "sta";
        break;
        case Mode::SNIFFER:
            m_str = "sniffer";
        break;
        default:
            m_str = "unknown";
        break;
    }

    return m_str;
}

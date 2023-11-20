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


WiFiTool::WiFiTool()
{
}
WiFiTool::~WiFiTool()
{

}
bool WiFiTool::Init()
{
    return true;
}

std::string RunScripts(const std::string& args) {
    auto        cmd = WIFI_SCRIPTS + " '" + args + "'";
    std::string output;
    ::Run(cmd, output);
    return output;
}
std::string WiFiTool::WifiOperate(const proto::WirelessCtrl& wifi_ctrl)
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
            result = "OK";
        }
        break;
        case ::proto::WirelessInfoType::WIFI_DISCONNECT:{
            Disconnect();
            result = "OK";
        }
        break;
        case ::proto::WirelessInfoType::WIFI_CHECK_CONNECTION:{
            if(CheckConnection()){
                result = "OK";
            }else{
                result = "Failed";
            }
        }
        break;
        case ::proto::WirelessInfoType::WIFI_TEST_SPEED:{
            float time_ms = GetAverageTime();
            auto result = std::to_string(time_ms);
        }
        break;
        case ::proto::WirelessInfoType::WIFI_CONNECT:{
            if(wifi_ctrl.wifi_ssid().empty()){
                result = "ssid incorrect";
            }else{
                ConnectWifi(wifi_ctrl.wifi_ssid(), wifi_ctrl.wifi_pswd());
                result = "OK";
            }
        }
        break;
        case ::proto::WirelessInfoType::WIFI_CREATE_AP:{
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
std::string WiFiTool::help() const {
    return RunScripts("-h");
}
std::string WiFiTool::version() const {
    return RunScripts("-v");
}

Mode WiFiTool::GetCurrentMode() const {
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
std::string WiFiTool::GetStaIp() const {
    return RunScripts("ip");
}
std::string WiFiTool::GetStaMac() const {
    return RunScripts("mac");
}
std::string WiFiTool::GetDns() const {
    return RunScripts("dns");
}
void WiFiTool::WlanUp() const {
    RunScripts("up");
}
void WiFiTool::WlanDown() const {
    RunScripts("down");
}
std::string WiFiTool::GetWpaInfo() const {
    return RunScripts("info");
}
std::string WiFiTool::GetWifiName() const {
    return RunScripts("name");
}
std::string WiFiTool::GetRouterMac() const {
    return RunScripts("bssid");
}
std::string WiFiTool::ConnectWifi(const std::string& ssid, const std::string& pwd) const {
    auto ssid_copy = ssid;
    auto pwd_copy  = pwd;
    ReplaceString(ssid_copy, "'", "\\'");
    ReplaceString(pwd_copy, "'", "\\'");

    auto        cmd = WIFI_SCRIPTS + " 'conn' " + ssid_copy +" " + pwd_copy;
    ADEBUG << "conn cmd: -----------" << cmd;
    std::string output;
    ::Run(cmd, output);
    return output;
}
std::string WiFiTool::Disconnect() const {
    return RunScripts("break");
}
std::string WiFiTool::CreateAp(const std::string& ssid, const std::string& pwd) const {
    auto ssid_copy = ssid;
    auto pwd_copy  = pwd;
    ReplaceString(ssid_copy, "'", "\\'");
    ReplaceString(pwd_copy, "'", "\\'");
    auto        cmd = WIFI_SCRIPTS + " 'ap' " + ssid_copy +" " + pwd_copy;
    ADEBUG << "create ap cmd: -----------" << cmd;
    std::string output;
    ::Run(cmd, output);
    return output;
}
std::string WiFiTool::Status() const
{
    return RunScripts("status_json");
}
std::string WiFiTool::List() const
{
    return RunScripts("list");
}
void WiFiTool::Reconnect() const {  // reconnect
    RunScripts("reconnect");
}

//------------------------------------------
bool WiFiTool::CheckConnection() const {
    auto output = RunScripts("ping");

    std::cout << "ping: " << output << std::endl;

    if ("网络连接正常" == output) {
        return true;
    }
    else {
        return false;
    }
}

float WiFiTool::GetAverageTime() const {
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
std::string WiFiTool::Mode2Str(const Mode& mode){
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

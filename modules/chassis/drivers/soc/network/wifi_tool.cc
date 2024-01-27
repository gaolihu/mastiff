#include "wifi_tool.h"
#include "cyber/cyber.h"

#include "utils.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace /*mstf::chss::*/proto;

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
    Run(cmd, output);
    return output;
}
std::string WiFiTool::WiFiOperate(const proto::WifiSetting& wifi_ctrl)
{
    std::string result="";
    switch( wifi_ctrl.wifi_func()){
        case E_WIFI_GET_MODE: {
            result = Mode2Str(GetCurrentMode());
        }
        break;
        case E_WIFI_GET_IP:{
            result = GetStaIp();
        }
        break;
        case E_WIFI_GET_MAC:{
            result = GetStaMac();
        }
        break;
        case E_WIFI_GET_DNS:{
            result = GetDns();
        }
        break;
        case E_WIFI_GET_WPA_INFO:{
            result = GetWpaInfo();
        }
        break;
        case E_WIFI_GET_NAME:{
            result = GetWiFiName();
        }
        break;
        case E_WIFI_GET_ROUTER_MAC:{
            result = GetRouterMac();
        }
        break;
        case E_WIFI_GET_STATUS:{
            result = Status();
        }
        break;
        case E_WIFI_GET_SCAN_LIST:{
            result = List();
        }
        break;
        case E_WIFI_GET_RECONNECT:{
            Reconnect();
            result = "OK";
        }
        break;
        case E_WIFI_DISCONNECT:{
            Disconnect();
            result = "OK";
        }
        break;
        case E_WIFI_CHECK_CONNECTION:{
            if(CheckConnection()){
                result = "OK";
            }else{
                result = "Failed";
            }
        }
        break;
        case E_WIFI_TEST_SPEED:{
            float time_ms = GetAverageTime();
            auto result = std::to_string(time_ms);
        }
        break;
        case E_WIFI_CONNECT:{
            if(wifi_ctrl.auth_info().has_ssid()){
                result = "ssid incorrect";
            }else{
                ConnectWiFi(wifi_ctrl.auth_info().ssid().value(),
                        wifi_ctrl.auth_info().pswd().value());
                result = "OK";
            }
        }
        break;
        case E_WIFI_CREATE_AP:{
            if(wifi_ctrl.auth_info().has_ssid()){
                result = "ssid incorrect";
            }else{
                CreateAp(wifi_ctrl.auth_info().ssid().value(),
                        wifi_ctrl.auth_info().pswd().value());
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
void WiFiTool::GenUpdateMsg(proto::WifiInfo& msg)
{
    auto mode = GetCurrentMode();
    msg.set_mode(Mode2Str(mode));

    if(mode != Mode::STA){
        return;
    }

    msg.set_ip(GetStaIp());
    msg.set_mac(GetStaMac());
    msg.set_dns(GetDns());
    msg.set_info(GetWpaInfo());
    msg.set_name(GetWiFiName());
    msg.set_bssid(GetRouterMac());
    // status
    msg.set_status(Status());
    // wifi list
    msg.set_scan_list(List());

    //TODO:
    // is oneline
    // ping speed
}

Mode WiFiTool::GetCurrentMode() const {
    auto        cmd = WIFI_SCRIPTS + " mode";
    std::string output;
    Run(cmd, output);
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
std::string WiFiTool::GetWiFiName() const {
    return RunScripts("name");
}
std::string WiFiTool::GetRouterMac() const {
    return RunScripts("bssid");
}
std::string WiFiTool::ConnectWiFi(const std::string& ssid, const std::string& pwd) const {
    auto ssid_copy = ssid;
    auto pwd_copy  = pwd;
    ReplaceString(ssid_copy, "'", "\\'");
    ReplaceString(pwd_copy, "'", "\\'");

    auto        cmd = WIFI_SCRIPTS + " 'conn' " + ssid_copy +" " + pwd_copy;
    ADEBUG << "conn cmd: -----------" << cmd;
    std::string output;
    Run(cmd, output);
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
    AINFO << "create ap cmd: -----------" << cmd;
    std::string output;
    Run(cmd, output);
    return output;
}
std::string WiFiTool::Status() const
{
    return RunScripts("status");
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
    Run(cmd, output);

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

}  // namespace driver
}  // namespace chss
}  // namespace mstf

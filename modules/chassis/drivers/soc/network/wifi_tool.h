#pragma once

#include "modules/chassis/proto/chss_io.pb.h"
#include "wifi_ap.h"
#include "wifi_sta.h"


namespace mstf {
namespace chss {
namespace driver {
const static std::string WIFI_SCRIPTS = "/userdata/wifi/wifi_tool.sh";

enum class Mode { NONE, AP, STA, SNIFFER };


class WiFiTool
    : public WiFiSta
    , WiFiAp , std::enable_shared_from_this<WiFiTool> {
public:
    WiFiTool();
    ~WiFiTool();
    bool Init();
    /**
     * @brief do wifi operate from proto message
     *
     * @param wifi_ctrl
     * @return std::string
     */
    std::string WiFiOperate(const proto::WifiSetting& wifi_ctrl);
    inline std::shared_ptr<WiFiTool> ThisShared(){
        return shared_from_this();
    }

    // clang-format off
    std::string help() const; // -h
    std::string version() const; // -v
    Mode        GetCurrentMode() const; // mode
    std::string GetStaIp() const; // ip
    std::string GetStaMac() const; // mac
    std::string GetDns() const; // dns
    void        WlanUp() const; // up
    void        WlanDown() const; // down
    std::string GetWpaInfo() const; // info
    std::string GetWiFiName() const; // name
    std::string GetRouterMac() const; // bssid
    std::string ConnectWiFi(const std::string& ssid, const std::string& pwd = "") const; // conn
    std::string Disconnect() const; // dis
    std::string CreateAp(const std::string& ssid, const std::string& pwd = "") const; // ap
    std::string Status() const; // status
    // scan
    std::string List() const;// list
    void Reconnect() const; // reconnect
    // clang-format on

    //----------------------------------------------------------
    //以下程序运行会有延时几秒
    // 通过ping 判断是否能连接外网
    bool  CheckConnection() const;  // ping
    // 获取ping baidu.com 的平均延迟, 单位 ms
    float GetAverageTime() const;  // speed

    void GenUpdateMsg(proto::WifiInfo& msg);
    std::string Mode2Str(const Mode& mode);

private:
};
}  // namespace driver
}  // namespace chss
}  // namespace mstf

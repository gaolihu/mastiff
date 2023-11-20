/*
 * @Date: 2023-09-13 11:29:18
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/network/wifi_tool.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#pragma once

#include "modules/chassis/proto/frame_down_stream.pb.h"
#include "modules/chassis/proto/frame_up_stream.pb.h"
#include "wifi_ap.h"
#include "wifi_sta.h"


namespace mstf {
namespace chss {
namespace network {
const static std::string WIFI_SCRIPTS = "/userdata/wifi/wifi_tool.sh";

enum class Mode { NONE, AP, STA, SNIFFER };


class WiFiTool
    : public WifiSta
    , WifiAp , std::enable_shared_from_this<WiFiTool> {
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
    std::string WifiOperate(const proto::WirelessCtrl& wifi_ctrl);
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
    std::string GetWifiName() const; // name
    std::string GetRouterMac() const; // bssid
    std::string ConnectWifi(const std::string& ssid, const std::string& pwd = "") const; // conn
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
    // 获取ping的平均延迟, 单位 ms
    float GetAverageTime() const;  // speed

private:
    std::string Mode2Str(const Mode& mode);
};
}  // namespace network
}  // namespace chss
}  // namespace mstf

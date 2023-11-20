/*
 * @Date: 2023-11-13 15:14:00
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/network/wifi_thread.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
#pragma once

#include <unistd.h>

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#include "cyber/common/log.h"
#include "modules/chassis/drivers/soc/soc_defs.h"
#include "modules/chassis/proto/frame_down_stream.pb.h"
#include "modules/chassis/proto/frame_up_stream.pb.h"
#include "modules/chassis/drivers/soc/network/wifi_tool.h"

namespace mstf {
namespace chss {
namespace network {

using SocDataPublisher = driver::SocDataListener;

class WiFiThread : public std::enable_shared_from_this<WiFiThread> {
public:
    WiFiThread();
    ~WiFiThread();
    void RegistePublisher(SocDataPublisher f);
    void PublishMsg(const protobuf::Message& msg);
    void SetWiFiCtrl(const proto::WirelessCtrl& ctrl);

    void Init();
    void Start();
    void Stop();
    void Close();
    inline void SetWiFiTool(std::shared_ptr<WiFiTool> t){
        wifi_tool_ = t;
    }
    inline std::shared_ptr<WiFiThread> ThisShared(){
        return shared_from_this();
    }

private:
    void WifiRunner();

    std::mutex              wifi_mtx_;
    std::condition_variable wifi_cv_;
    bool                    can_running_{false};
    std::thread             wifi_th_;

    SocDataPublisher soc_publisher_;
    std::shared_ptr<WiFiTool> wifi_tool_;
    proto::WirelessCtrl wifi_ctrl_;
};
}  // namespace network
}  // namespace chss
}  // namespace mstf

#pragma once

#include <unistd.h>

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#include "cyber/common/log.h"
#include "cyber/timer/timer.h"
#include "cyber/timer/timer_task.h"
#include "modules/chassis/drivers/soc/network/wifi_tool.h"
#include "modules/chassis/proto/chss_io.pb.h"

namespace mstf {
namespace chss {
namespace driver {

//using SocDataPublisher = driver::SocDataListener;
using namespace google;
using namespace proto;

class WiFiThread : public std::enable_shared_from_this<WiFiThread> {
public:
    WiFiThread();
    ~WiFiThread();
    //void RegistePublisher(SocDataPublisher f);
    void PublishMsg(const protobuf::Message& msg);
    void SetWiFiCtrl(const proto::WifiSetting& ctrl);

    void        Init();
    void        Start();
    void        Stop();
    void        Close();
    inline void SetWiFiTool(std::shared_ptr<WiFiTool> t) {
        wifi_tool_ = t;
    }
    inline std::shared_ptr<WiFiThread> ThisShared() {
        return shared_from_this();
    }

private:
    void WiFiRunner();
    void OnTimerCb();

    std::mutex              wifi_mtx_;
    std::condition_variable wifi_cv_;
    bool                    wifi_running_{false};
    std::thread             wifi_th_;
    bool                    set_ctrl_cmd_{false};  // 设置命令
    bool                    update_notify_{false}; // 定时更新状态

    //SocDataPublisher          soc_publisher_;
    std::shared_ptr<WiFiTool> wifi_tool_;
    proto::WifiSetting       wifi_ctrl_;

    apollo::cyber::Timer timer_;
};
}  // namespace driver
}  // namespace chss
}  // namespace mstf

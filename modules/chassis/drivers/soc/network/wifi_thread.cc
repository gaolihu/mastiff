#include "modules/chassis/drivers/soc/network/wifi_thread.h"

namespace mstf {
namespace chss {
namespace driver {
WiFiThread::WiFiThread() {
    wifi_running_ = false;
}

WiFiThread::~WiFiThread() {
    Close();
}
void WiFiThread::Init() {
    wifi_tool_ = std::make_shared<WiFiTool>();

    apollo::cyber::TimerOption opt;
    opt.oneshot  = false;
    opt.period   = 10000;
    opt.callback = std::bind(&WiFiThread::OnTimerCb, this);
    timer_.SetTimerOption(opt);
}
void WiFiThread::Start() {
    if (wifi_running_) {
        AERROR << "wifi have started, can not start again!";
        return;
    }

    timer_.Start();

    wifi_running_ = true;

    wifi_th_ = std::thread(std::bind(&WiFiThread::WiFiRunner, this));
}
void WiFiThread::Stop() {
    {
        std::lock_guard<std::mutex> lg(wifi_mtx_);
        wifi_running_ = false;
        set_ctrl_cmd_ = true;
        wifi_ctrl_.Clear();
    }

    wifi_cv_.notify_one();

    if (wifi_th_.joinable())
        wifi_th_.join();

    timer_.Stop();
    AINFO << "stop the wifi thread finished";
}
void WiFiThread::Close() {
    Stop();
    AINFO << "close the wifi thread finished";
}
//void WiFiThread::RegistePublisher(SocDataPublisher f) {
    //soc_publisher_ = f;
    //TODO
//}
void WiFiThread::PublishMsg(const protobuf::Message& msg) {
    //if (soc_publisher_) {
        //soc_publisher_(msg);
    //} else {
        AERROR << "no publisher to pub wifi results!";
    //}
}
void WiFiThread::SetWiFiCtrl(const proto::WifiSetting& ctrl) {
    if (ctrl.wifi_func() == E_WIFI_CLOSE) {
        AINFO << "go to close wifi thread...";
        Close();
        return;
    }
    else if (ctrl.wifi_func() == E_WIFI_START) {
        AINFO << "go to start wifi thread...";
        Start();
        return;
    }

    {
        std::lock_guard<std::mutex> lk(wifi_mtx_);
        wifi_ctrl_    = ctrl;
        set_ctrl_cmd_ = true;
    }
    if (!wifi_running_) {
        AERROR << "wifi not running, can not set wifi control cmd";
        return;
    }

    wifi_cv_.notify_one();
}
void WiFiThread::OnTimerCb() {
    {
        std::lock_guard<std::mutex> lg(wifi_mtx_);
        update_notify_ = true;
    }

    wifi_cv_.notify_one();
}

void WiFiThread::WiFiRunner() {
    while (wifi_running_) {
        std::unique_lock<std::mutex> lk(wifi_mtx_);
        wifi_cv_.wait(lk, [this] {
            return !wifi_running_ || set_ctrl_cmd_ || update_notify_;
        });

        if (!wifi_running_) {
            break;
        }

        if (set_ctrl_cmd_) {
            auto result   = wifi_tool_->WiFiOperate(wifi_ctrl_);
            WifiInfo response;
            response.set_wifi_result(result);
            PublishMsg(response);
            set_ctrl_cmd_ = false;
        }

        if (update_notify_) {
            mstf::chss::proto::WifiInfo msg;
            wifi_tool_->GenUpdateMsg(msg);
            PublishMsg(msg);

            // reset condition flag
            update_notify_ = false;
        }

        AINFO << "wifi thread running once...";
    }
}
}  // namespace driver
}  // namespace chss
}  // namespace mstf

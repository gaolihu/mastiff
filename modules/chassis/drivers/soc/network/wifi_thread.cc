#include "modules/chassis/drivers/soc/network/wifi_thread.h"

namespace mstf {
namespace chss {
namespace driver {

WiFiThread::WiFiThread() {
    AINFO << "wifi thread";
}

WiFiThread::~WiFiThread() {
    Close();
}
void WiFiThread::Init() {
    wifi_tool_ = std::make_shared<WiFiTool>();
}

void WiFiThread::Start() { }
void WiFiThread::Stop() { }
void WiFiThread::Close() { }

void WiFiThread::SetWiFiCtrl(const proto::WifiSetting& ctrl) {
    set_ctrl_cmd_ = true;
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

    wifi_ctrl_    = ctrl;
}

void WiFiThread::PollingNetworkRutine(WifiInfo& rsp) {
    if (set_ctrl_cmd_) {
        auto result   = wifi_tool_->WiFiOperate(wifi_ctrl_);
        rsp.mutable_wifi_result()->set_value(result);
        set_ctrl_cmd_ = false;
    }

    if (has_feedback_++ % (int)1e8 == 0) {
        //update wifi info periodically, ~ 5.5s
        AINFO << "has_feedback_" << has_feedback_;
        wifi_tool_->GenUpdateMsg(rsp);
    }
}

}  // namespace driver
}  // namespace chss
}  // namespace mstf

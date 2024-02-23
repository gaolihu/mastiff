#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/drivers/soc/network/wifi_tool.h"
#include "modules/chassis/proto/chss_io.pb.h"

namespace mstf {
namespace chss {
namespace driver {

using namespace google;
using namespace proto;

class WiFiThread : public std::enable_shared_from_this<WiFiThread> {
public:
    WiFiThread();
    ~WiFiThread();
    void SetWiFiCtrl(const proto::WifiSetting& ctrl);

    void        Init();
    void        Start();
    void        Stop();
    void        Close();

    void PollingNetworkRutine(WifiInfo&);

private:
    std::shared_ptr<WiFiTool> wifi_tool_;
    proto::WifiSetting       wifi_ctrl_;
    bool set_ctrl_cmd_ { false };
    int has_feedback_ { 0 };
};

}  // namespace driver
}  // namespace chss
}  // namespace mstf

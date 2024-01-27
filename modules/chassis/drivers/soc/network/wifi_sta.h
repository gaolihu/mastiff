#pragma once

#include <iostream>
#include <string>
namespace mstf {
namespace chss {
namespace driver {
enum class StaPasswordState_t { UNKNOWN, CORRECT, INCORRECT };

class WiFiSta {
public:
    void        EnableSta(const std::string &ssid, const std::string &pwd);
    void        DisableSta() const;
    bool        CheckStaEnabled() const;
    std::string GetStaSsid() const;
    std::string GetStaPassword() const;
    int         GetStaChannel() const;
    std::string GetStaMac() const;

private:
    enum class WpaState_t {
        WPA_UNKNOWN      = -1,
        WPA_DISCONNECTED = 0,
        WPA_INTERFACE_DISABLED,
        WPA_INACTIVE,
        WPA_SCANNING,
        WPA_AUTHENTICATING,
        WPA_ASSOCIATING,
        WPA_ASSOCIATED,
        WPA_4WAY_HANDSHAKE,
        WPA_GROUP_HANDSHAKE,
        WPA_COMPLETED
    };

    std::string wlan_name_;
    std::string wpa_supplicant_conf_;

    int GetWpaSupplicantInfo(const std::string &key, std::string &val) const;

    WpaState_t GetWpaState() const;
};
}  // namespace driver
}  // namespace chss
}  // namespace mstf

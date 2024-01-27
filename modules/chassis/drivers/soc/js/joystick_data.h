#pragma once

#include <sys/epoll.h>
#include <linux/joystick.h>

#include "modules/chassis/proto/chassis_config.pb.h"

namespace mstf {
namespace chss {
namespace driver {

using namespace /*mstf::chss::*/proto;

#define MAX_EVENTS 20

class JoyStickData {
public:
    JoyStickData(const JoyConfigs*);
    ~JoyStickData();

    int Init();
    int Start();
    int Stop();
    int Resume();
    int Close();

    int WritePort(const uint8_t*, const size_t);

    void PollingJsRutine(std::vector<uint8_t>&);

    int               joy_fd_    = -1;
    const JoyConfigs*  joy_conf_  = nullptr;

    int                epoll_fd_;
    struct epoll_event event_;
    struct epoll_event events_list_[MAX_EVENTS];
    struct js_event    joystick_event_;
};

}  //namespace driver
}  //namespace chss
}  //namespace mstf

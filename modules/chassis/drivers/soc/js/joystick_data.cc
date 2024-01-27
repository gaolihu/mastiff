#include <fcntl.h>
#include <stdlib.h>

#include "cyber/common/log.h"

#include "modules/chassis/drivers/soc/js/joystick_data.h"

namespace mstf {
namespace chss {
namespace driver {

JoyStickData::JoyStickData(const JoyConfigs* c) {
#ifdef CHSS_PKG_DBG
    AINFO << "JoyStickData construct!";
#endif
    ACHECK(c) << "joy config null!";

    joy_conf_    = c;
}

JoyStickData::~JoyStickData() {
#ifdef CHSS_PKG_DBG
    AINFO << "JoyStickData de-construct!";
#endif
    Close();
}

int JoyStickData::Init() {
    //default handler for lower data upstream
    AINFO << "joy init: " << joy_conf_->dev_setting().joy_dev() <<
        ", freq: " << joy_conf_->dev_setting().input_read_freq()
          << "ms, once read: " << joy_conf_->dev_setting().once_read_size() << " Bytes";
    //DriveDataItf::Init(joy_conf_->dev_setting().joy_dev(),
            //cycle <= 0 ? joy_conf_->dev_setting().input_read_freq() : cycle);

    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        AFATAL << "Epoll for joystick creation failed";
    }
    event_.events = EPOLLIN | EPOLLET;  // EPOLLET for edge-triggered mode

    return 0;
}

//send to serail device
int JoyStickData::WritePort(const uint8_t* data, const size_t len) {
    if (joy_fd_ <= 0) {
        //do not write when joy doesn't open
        AWARN << "write? joy fd: " << joy_fd_ << ", joy liseter ?";
        return 0;
    }

    int count = write(joy_fd_, data, len);

    if (count < 0) {
        AFATAL << "write joy device failed!";
    }
    return count;
}

int JoyStickData::Start() {
    if (access(joy_conf_->dev_setting().joy_dev().c_str(), R_OK) != 0) {
        AFATAL << "device: " << joy_conf_->dev_setting().joy_dev() << " can not be read!";
        return -1;
    }

    if (joy_fd_ > 0) {
        AWARN << "device: " << joy_conf_->dev_setting().joy_dev() << ", already opened!";
        return 0;
    }
    else {
        //for some situations, can't get data from joy
        joy_fd_ = open(joy_conf_->dev_setting().joy_dev().c_str(), O_RDONLY | O_NONBLOCK);
        if (joy_fd_ <= 0) {
            AFATAL << "open joy device " << joy_conf_->dev_setting().joy_dev() << " failed!" << joy_fd_;
            return -1;
        }
        close(joy_fd_);
        joy_fd_ = -1;

        joy_fd_ = open(joy_conf_->dev_setting().joy_dev().c_str(), O_RDONLY | O_NONBLOCK);

        if (joy_fd_ <= 0) {
            AFATAL << "open joy device " << joy_conf_->dev_setting().joy_dev() << " failed!" << joy_fd_;
            return -1;
        }

        event_.data.fd = joy_fd_;
    }

    // epoll ctrl
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, joy_fd_, &event_) == -1) {
        AFATAL << "Epoll_ctl for joy failed";
        return -1;
    }

    AINFO << "Start " << joy_conf_->dev_setting().joy_dev() << ", fd: " << joy_fd_ << " OK";

    return 0;
}

int JoyStickData::Stop() {
    AINFO << "Stop JoyStickData, dev: " << joy_conf_->dev_setting().joy_dev();

    return 0;
}

int JoyStickData::Resume() {
    AINFO << "Resume JoyStickData, dev: " << joy_conf_->dev_setting().joy_dev();

    return 0;
}

int JoyStickData::Close() {
    AINFO << "Close JoyStickData, dev: " << joy_conf_->dev_setting().joy_dev();

    if (joy_fd_ > 0) {
        close(joy_fd_);
        joy_fd_ = -1;
    }
    else {
        AWARN << joy_conf_->dev_setting().joy_dev() << " not opened, can't be closed!";
    }

    return 0;
}

void JoyStickData::PollingJsRutine(std::vector<uint8_t>& data) {
    // wait epoll, nonblock
    int event_nums = epoll_wait(epoll_fd_, events_list_, MAX_EVENTS, -1);
    if (event_nums == -1) {
        AERROR << "Epoll wait failed";
        return;
    }

    for (int i = 0; i < event_nums; ++i) {
        if (events_list_[i].data.fd == joy_fd_) {
            while (1) {
                if (events_list_[i].events & EPOLLERR || events_list_[i].events & EPOLLHUP || !(events_list_[i].events & EPOLLIN)) {
                    AERROR << "Error, joy input read events error: " << events_list_[i].events;
                    continue;
                }

                ssize_t bytes_read = read(joy_fd_, &joystick_event_, sizeof(js_event));

                if (bytes_read == -1) {
                    break;
                }

                // Handle the joystick event
                if (bytes_read == sizeof(js_event)) {
#ifdef CHSS_PKG_DBG
                    //AINFO_EVERY(20) << "Read JoyStick bytes len: " << bytes_read;
                    //AINFO_EVERY(20) << "Event: type=" << (int)joystick_event_.type << " time=" << joystick_event_.time
                                    //<< " number=" << (int)joystick_event_.number << " value=" << joystick_event_.value;
#endif
                    data.insert(data.end(), (uint8_t*)&joystick_event_,
                            (uint8_t*)((uint8_t*)&joystick_event_ + bytes_read));
                }
            }
        }
    }
}

}  //namespace driver
}  //namespace chss
}  //namespace mstf

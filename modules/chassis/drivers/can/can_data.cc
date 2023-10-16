#include <iomanip>
#include <unistd.h>

#include "cyber/common/log.h"

#include "modules/chassis/drivers/can/can_data.h"
#include "modules/chassis/drivers/can/comm/can_ops.h"

namespace mstf {
namespace chss {
namespace driver {

    CanData::CanData(const DriveDataMonitorCan& m,
            const proto::CanConfig* c) {
#ifdef CHSS_PKG_DBG
            AINFO << "CanData construct" <<
#if 0
                c->DebugString();
#else
                "";
#endif
#endif
        // =&RawManage::OnRecvCan()
        can_monitor_ = m;
        can_conf_ = c;
    }

    CanData::~CanData() {
#ifdef CHSS_PKG_DBG
            AINFO << "CanData de-construct!";
#endif
        Close();
    }

    int CanData::Init(const int cycle,
            const DriveDataPolling& p) {
        if (p != nullptr) {
            AINFO << "Init can driver with upper poller, cycle ms: " <<
                (cycle == 0 ? can_conf_->dev_setting().can_read_freq() : cycle);
            DriveDataItf::Init(cycle == 0 ?
                    can_conf_->dev_setting().can_read_freq() : cycle, p);
        } else {
            //default handler for lower data upstream
            AINFO << "Init can driver with default poller, cycle ms: " <<
                (cycle == 0 ? can_conf_->dev_setting().can_read_freq() : cycle);
            DriveDataItf::Init(cycle == 0 ?
                    can_conf_->dev_setting().can_read_freq() : cycle,
                    std::bind(&CanData:: can_poll_func, this));
        }

        return 0;
    }

    int CanData::Start() {
        AINFO << "start can data driver for " <<
            can_conf_->dev_setting().dev();

        if ((can_socket_fd_ = can_open()) < 0) {
            AERROR << "open " <<
                can_conf_->dev_setting().dev() <<
                " failed!";
            return -1;
        } else {
            AINFO << "open " <<
                can_conf_->dev_setting().dev() <<
                " socket fd: " << can_socket_fd_;
        }

        if (can_config(can_socket_fd_, can_conf_->dev_setting().baud(),
                    can_conf_->dev_setting().can_read_timeout()) != 0) {
            AERROR << "config " <<
                can_conf_->dev_setting().dev() <<
                " failed!";
            return -1;
        } else {
            AINFO << "config OK " <<
                can_conf_->dev_setting().dev() <<
                ", baud: " << can_conf_->dev_setting().baud() <<
                ", poll timeout ms: " << can_conf_->dev_setting().can_read_timeout();
        }

        return DriveDataItf::Start();
    }

    int CanData::Stop() {
        AINFO << "Stop CanData: " <<
            can_conf_->dev_setting().dev();

        return DriveDataItf::Stop();
    }

    int CanData::Close() {
        AINFO << "Close CanData: " <<
            can_conf_->dev_setting().dev();

        Stop();

        if (can_socket_fd_ > 0) {
            can_close(can_socket_fd_);
            can_socket_fd_ = -1;
            return 0;
        } else {
            AWARN << can_conf_->dev_setting().dev() <<
                " not opened, can't be closed!";
        }

        return DriveDataItf::Close();
    }

    int CanData::WritePort(const std::vector<uint8_t>& msg) {
        if (can_socket_fd_ <= 0) {
            AERROR << "send can string message without dev";
            return -1;
        }

        if (can_send_string(can_socket_fd_,
                    can_conf_->dev_setting().dev().data(),
                    (const char*)(&msg[0])) != 0) {
            AWARN << "can send string failed to " <<
                can_conf_->dev_setting().dev() <<
                ", len: " << msg.size();
            return -1;
        }

        return msg.size();
    }

    int CanData::WritePort(const int id, const uint8_t* data,
            const size_t size) {
#if 0
        AINFO << "write can port, id: " << std::hex << id <<
            ", [ " << std::setw(2) << std::setfill('0') << static_cast<int>(data[0]) <<
            " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[1]) <<
            " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[2]) <<
            " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[3]) <<
            " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[4]) <<
            " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[5]) <<
            " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[6]) <<
            " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[7]) <<
            " ]";
#endif
        if (can_socket_fd_ <= 0) {
            AERROR << "send can raw message without dev";
            return -1;
        }

        if (can_send_raw(id, data, size) != 0) {
            AWARN << "can send raw failed to " <<
                can_conf_->dev_setting().dev() <<
                ", len: " << size;
            return -1;
        }

        return size;
    }

    void CanData::can_poll_func() {
        //get can data
        int len = 0;
        char buf[512] = {0};
        if ((len = can_recv(can_socket_fd_,
                        can_conf_->dev_setting().
                        dev().data(), buf)) > 0) {
            if (can_monitor_ == nullptr) {
                AERROR << "no can-monitor!?";
            } else {
                can_monitor_((const uint8_t*)buf, len);
            }
        }
    }

} // namespace driver
} // namespace chss
} // namespace mstf

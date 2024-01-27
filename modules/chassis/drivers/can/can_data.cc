#include <iomanip>
#include <unistd.h>

#include "cyber/common/log.h"

#include "modules/chassis/drivers/can/can_data.h"
#include "modules/chassis/drivers/can/comm/can_ops.h"

namespace mstf {
namespace chss {
namespace driver {

    CanData::CanData(const DriveDataMonitor& m,
            const CanConfigs* c,
            const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
            AINFO << "CanData construct" <<
#if 0
                c->DebugString();
#else
                "";
#endif
#endif
        // =&ParseDrvLink::OnRecvCan()
        data_monitor_ = m;
        can_conf_ = c;
        s_idc_ = si;
    }

    CanData::~CanData() {
#ifdef CHSS_PKG_DBG
            AINFO << "CanData de-construct!";
#endif
        Close(s_idc_);
    }

    int CanData::Init(const std::string& name,
            const int cycle) {
            //default handler for lower data upstream
        AINFO << "Init can driver with default poller, cycle ms: " <<
            (cycle <= 0 ? can_conf_->dev_setting().can_read_freq() : cycle);
        DriveDataItf::Init(can_conf_->dev_setting().dev(),
                cycle <= 0 ? can_conf_->dev_setting().can_read_freq() : cycle);

        return 0;
    }

    int CanData::Init(const SensorIndicator* si) {
        return DriveDataItf::Init(can_conf_->dev_setting().dev(),
                can_conf_->dev_setting().can_read_freq());
    }

    int CanData::Start(const SensorIndicator* si) {
        AINFO << "start can data driver for " <<
            can_conf_->dev_setting().dev();

        if ((dev_fd_ = can_open()) < 0) {
            AERROR << "open " <<
                can_conf_->dev_setting().dev() <<
                " failed!";
            return -1;
        } else {
            AINFO << "open " <<
                can_conf_->dev_setting().dev() <<
                " socket fd: " << dev_fd_;
        }

        if (can_config(dev_fd_, can_conf_->dev_setting().baud(),
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

    int CanData::Stop(const SensorIndicator* si) {
        AINFO << "Stop CanData: " <<
            can_conf_->dev_setting().dev();

        return DriveDataItf::Stop();
    }

    int CanData::Resume(const SensorIndicator* si) {
        AINFO << "Resume CanData: " <<
            can_conf_->dev_setting().dev();

        return DriveDataItf::Resume();
    }

    int CanData::Close(const SensorIndicator* si) {
        AINFO << "Close CanData: " <<
            can_conf_->dev_setting().dev();

        Stop(s_idc_);

        if (dev_fd_ > 0) {
            can_close(dev_fd_);
            dev_fd_ = -1;
        } else {
            AWARN << can_conf_->dev_setting().dev() <<
                " not opened, can't be closed!";
        }

        return DriveDataItf::Close();
    }

    int CanData::WritePort(const std::vector<uint8_t>& msg) {
        if (dev_fd_ <= 0) {
            AERROR << "send can string message without dev";
            return -1;
        }

        if (can_send_string(dev_fd_,
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
        if (dev_fd_ <= 0) {
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

    void CanData::PollingDriveRutine() {
        //get can data
        int len = 0;
        char buf[512] = {0};
        if ((len = can_recv(dev_fd_,
                        can_conf_->dev_setting().
                        dev().data(), buf)) > 0) {
            if (data_monitor_ == nullptr) {
                AERROR << "no can-monitor!?";
            } else {
                data_monitor_(s_idc_, (const uint8_t*)buf, len);
            }
        }
    }

} // namespace driver
} // namespace chss
} // namespace mstf

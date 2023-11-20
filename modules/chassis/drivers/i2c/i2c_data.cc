#include "cyber/common/log.h"

//#include "modules/chassis/drivers/i2c/i2c_drive.h"
#include "modules/chassis/drivers/i2c/i2c_data.h"

namespace mstf {
namespace chss {
namespace driver {

    IIcData::IIcData(const DriveDataMonitor& m,
            const I2cConfig* c,
            const SensorInfo* si) {
#ifdef CHSS_PKG_DBG
            AINFO << "IIcData construct!";
#endif
        ACHECK(c) << "i2c config null!";

        i2c_monitor_ = m;
        i2c_conf_ = c;
        snsr_info_ = si;
    }

    IIcData::~IIcData() {
#ifdef CHSS_PKG_DBG
            AINFO << "IIcData de-construct!";
#endif
        Close();
    }

    int IIcData::Init(const std::string& name,
            const int cycle,
            const DriveDataPolling& p) {
        AINFO << "i2c init: " << i2c_conf_->dev_setting().i2c_dev() <<
            ", bus: " << i2c_conf_->dev_setting().bus() <<
            ", address: " << i2c_conf_->dev_setting().address() <<
            ", read freq: " << i2c_conf_->dev_setting().i2c_read_freq() <<
            ", timeout: " << i2c_conf_->dev_setting().i2c_read_timeout();

        if (p != nullptr) {
            AINFO << "init i2c driver with upper poller, cycle ms: " <<
                (cycle == 0 ? i2c_conf_->dev_setting().i2c_read_freq() : cycle);
            DriveDataItf::Init(name.empty() ? i2c_conf_->dev_setting().i2c_dev(): name,
                    cycle == 0 ? i2c_conf_->dev_setting().i2c_read_freq() : cycle, p);
        } else {
            AINFO << "init i2c driver with default poller, cycle ms: " <<
                (cycle == 0 ? i2c_conf_->dev_setting().i2c_read_freq() : cycle);
            DriveDataItf::Init(name.empty() ? i2c_conf_->dev_setting().i2c_dev(): name,
                    cycle == 0 ? i2c_conf_->dev_setting().i2c_read_freq() : cycle,
                    std::bind(&IIcData:: i2c_poll_func, this));
        }

        return 0;
    }

    //send to i2c device
    int IIcData::Push(const uint8_t* data, const size_t len) {
#ifdef i2c_DBG
        std::ostringstream os;
        std::ostringstream oss;
        for(int i = 0; i < (int)len; i++) {
            oss << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<int>(data[i]) << " ";
        }
        os.str("");
        os << std::dec << std::setw(3) << std::setfill('0') << len;
        AINFO << "send i2c data, len: " <<
            os.str() << " -> " << i2c_conf_->dev_setting().i2c_dev() <<
            ", [ " << oss.str() << " ]";
#endif
        int count = 0;
        if (i2c_fd_ <= 0 || i2c_monitor_ == nullptr) {
            //do not write when i2c doesn't open
            AWARN << "write? i2c fd: " << i2c_fd_ <<
                ", i2c liseter ?";
            return 0;
        }

        //count = write_i2c(i2c_fd_, data, len);

        if(count < 0) {
            AFATAL << "write i2c device failed!";
        }
#ifdef i2c_DBG
        else {
            AINFO << "write i2c OK: " << count;
        }
#endif
        return count;
    }

    int IIcData::Start() {
        if (access(i2c_conf_->dev_setting().i2c_dev().c_str(), R_OK) != 0) {
            AFATAL << "device: " << i2c_conf_->dev_setting().i2c_dev() <<
                "can not be read!";
            return -1;
        }

        if (i2c_fd_ > 0) {
            AWARN << "device: " << i2c_conf_->dev_setting().i2c_dev() <<
                ", already opened!";
        } else {
            //for some situations, can't get data from i2c
            /*
            i2c_fd_ = open_i2c(i2c_conf_->dev_setting().i2c_dev().c_str(),
                    i2c_conf_->dev_setting().bus(),
                    i2c_conf_->dev_setting().address(),
                    i2c_conf_->dev_setting().i2c_read_timeout());
                    */
            i2c_fd_ = 100;
            if (i2c_fd_ <= 0) {
                AFATAL << "open i2c device " <<
                    i2c_conf_->dev_setting().i2c_dev() <<
                    " failed!" << i2c_fd_;
                return -1;
            }
            //close_i2c(i2c_fd_);
            i2c_fd_ = 100;

            /*
            i2c_fd_ = open_i2c(i2c_conf_->dev_setting().i2c_dev().c_str(),
                    i2c_conf_->dev_setting().bus(),
                    i2c_conf_->dev_setting().address(),
                    i2c_conf_->dev_setting().i2c_read_timeout());
                    */

            if (i2c_fd_ <= 0) {
                AFATAL << "open i2c device " <<
                    i2c_conf_->dev_setting().i2c_dev() <<
                    " failed!" << i2c_fd_;
                return -1;
            }
        }

        AINFO << "Start " << i2c_conf_->dev_setting().i2c_dev() <<
            ", fd: " << i2c_fd_ <<
            ", bus: " << i2c_conf_->dev_setting().bus() <<
            ", address: " << i2c_conf_->dev_setting().address();

        return DriveDataItf::Start();
    }

    int IIcData::Stop() {
        AINFO << "Stop IIcData, dev: " <<
            i2c_conf_->dev_setting().i2c_dev();

        return DriveDataItf::Stop();
    }

    int IIcData::Resume() {
        AINFO << "Resume IIcData, dev: " <<
            i2c_conf_->dev_setting().i2c_dev();

        return DriveDataItf::Resume();
    }

    int IIcData::Close() {
        AINFO << "Close IIcData, dev: " <<
            i2c_conf_->dev_setting().i2c_dev();

        if (i2c_fd_ > 0) {
            //close_i2c(i2c_fd_);
            i2c_fd_ = -1;
        } else {
            AWARN << i2c_conf_->dev_setting().i2c_dev() <<
                " not opened, can't be closed!";
        }

        DriveDataItf::Stop();
        return DriveDataItf::Close();
    }

    void IIcData::i2c_poll_func() {
        int len = 1;
        uint8_t buf[16] = {0};
        if (i2c_monitor_ == nullptr)
            AFATAL << "no i2c listener";

        //if ((len = read_i2c(i2c_fd_, buf, 16)) != 16) { 
        if (1) {
#ifdef i2c_DBG
            std::ostringstream os;
            std::ostringstream oss;
            for(int i = 0; i < len; i++) {
                oss << std::hex << std::setw(2) << std::setfill('0') <<
                    static_cast<int>(buf[i]) << " ";
            }
            os.str("");
            os << std::dec << std::setw(3) << std::setfill('0') << len;
            AINFO << "get i2c data, len: " <<
                os.str() << " <- " << i2c_conf_->dev_setting().i2c_dev() <<
                ", [ " << oss.str() << " ]";
#endif
            i2c_monitor_((const void*)snsr_info_, buf, len);
        } else {
#ifdef i2c_DBG
            AWARN_EVERY(200) << "read dev: " << i2c_conf_->dev_setting().i2c_dev() <<
                ", len: " << len <<
                ", no data, ignore!!";
#endif
        }
    }

} //namespace driver
} //namespace chss
} //namespace mstf

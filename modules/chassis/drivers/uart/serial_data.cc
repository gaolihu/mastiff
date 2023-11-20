#include "cyber/common/log.h"

#include "modules/chassis/drivers/uart/uart_drive.h"
#include "modules/chassis/drivers/uart/serial_data.h"

namespace mstf {
namespace chss {
namespace driver {

    SerialData::SerialData(const DriveDataMonitor& m,
            const UartConfig* c,
            const SensorInfo* si) {
#ifdef CHSS_PKG_DBG
            AINFO << "SerialData construct!";
#endif
        ACHECK(c) << "uart config null!";

        serial_monitor_ = m;
        uart_conf_ = c;
        snsr_info_ = si;
    }

    SerialData::~SerialData() {
#ifdef CHSS_PKG_DBG
            AINFO << "SerialData de-construct!";
#endif
        Close();
    }

    int SerialData::Init(const std::string& name,
            const int cycle,
            const DriveDataPolling& p) {
#ifdef SERIAL_PORTING
        comm_ = new Serial(uart_conf_->dev_setting().uart_dev(),
                uart_conf_->dev_setting().baud(),
                Timeout::simpleTimeout(DEFAULT_TIMEOUT));
        //bind
        comm_->bindport(uart_conf_->dev_setting().uart_dev().data(),
                uart_conf_->dev_setting().baud());
#else
        AINFO << "serial init: " << uart_conf_->dev_setting().uart_dev() <<
            ", baud: " << uart_conf_->dev_setting().baud() <<
            ", datab: " << uart_conf_->dev_setting().data_bits() <<
            ", stopb: " << uart_conf_->dev_setting().stop_bits() <<
            ", parity: " << uart_conf_->dev_setting().parity() <<
            ", freq: " << uart_conf_->dev_setting().serial_read_freq() <<
            ", timeout: " << uart_conf_->dev_setting().serial_read_timeout() <<
            "ms, once read: " << uart_conf_->dev_setting().once_read_size() <<
            " Bytes";

        if (p != nullptr) {
            AINFO << "init serial driver with upper poller, cycle ms: " <<
                (cycle == 0 ? uart_conf_->dev_setting().serial_read_freq() : cycle);
            DriveDataItf::Init(name.empty() ? uart_conf_->dev_setting().uart_dev(): name,
                    cycle == 0 ? uart_conf_->dev_setting().serial_read_freq() : cycle, p);
        } else {
            AINFO << "init serial driver with default poller, cycle ms: " <<
                (cycle == 0 ? uart_conf_->dev_setting().serial_read_freq() : cycle);
            DriveDataItf::Init(name.empty() ? uart_conf_->dev_setting().uart_dev(): name,
                    cycle == 0 ? uart_conf_->dev_setting().serial_read_freq() : cycle,
                    std::bind(&SerialData:: serial_poll_func, this));
        }
#endif

        return 0;
    }

    //send to serail device
    int SerialData::Push(const uint8_t* data, const size_t len) {
#ifdef SERIAL_DBG
        std::ostringstream os;
        std::ostringstream oss;
        for(int i = 0; i < (int)len; i++) {
            oss << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<int>(data[i]) << " ";
        }
        os.str("");
        os << std::dec << std::setw(3) << std::setfill('0') << len;
        AINFO << "send serial data, len: " <<
            os.str() << " -> " << uart_conf_->dev_setting().uart_dev() <<
            ", [ " << oss.str() << " ]";
#endif
        int count = 0;
#ifdef SERIAL_PORTING
        count = comm_->writeData(data, len);
#else
        if (serial_fd_ <= 0 || serial_monitor_ == nullptr) {
            //do not write when serial doesn't open
            AWARN << "write? serial fd: " << serial_fd_ <<
                ", uart liseter ?";
            return 0;
        }

        count = write_uart(serial_fd_, data, len);

        if(count < 0) {
            AFATAL << "write uart device failed!";
        }
#ifdef SERIAL_DBG
        else {
            AINFO << "write uart OK: " << count;
        }
#endif
#endif
        return count;
    }

    int SerialData::Start() {
        if (access(uart_conf_->dev_setting().uart_dev().c_str(), R_OK) != 0) {
            AFATAL << "device: " << uart_conf_->dev_setting().uart_dev() <<
                "can not be read!";
            return -1;
        }

#ifdef SERIAL_PORTING
        //open
        if (!comm_->open()) {
            return -1;
        }
#else
        if (serial_fd_ > 0) {
            AWARN << "device: " << uart_conf_->dev_setting().uart_dev() <<
                ", already opened!";
        } else {
            //for some situations, can't get data from uart
            serial_fd_ = open_uart(uart_conf_->dev_setting().uart_dev().c_str(),
                    uart_conf_->dev_setting().baud(), 0, 0, 0,
                    uart_conf_->dev_setting().serial_read_timeout());
            if (serial_fd_ <= 0) {
                AFATAL << "open uart device " <<
                    uart_conf_->dev_setting().uart_dev() <<
                    " failed!" << serial_fd_;
                return -1;
            }
            close_uart(serial_fd_);
            serial_fd_ = -1;

            serial_fd_ = open_uart(uart_conf_->dev_setting().uart_dev().c_str(),
                    uart_conf_->dev_setting().baud(),
                    uart_conf_->dev_setting().data_bits(),
                    uart_conf_->dev_setting().stop_bits(),
                    uart_conf_->dev_setting().parity(),
                    uart_conf_->dev_setting().serial_read_timeout());

            if (serial_fd_ <= 0) {
                AFATAL << "open uart device " <<
                    uart_conf_->dev_setting().uart_dev() <<
                    " failed!" << serial_fd_;
                return -1;
            }
        }
#endif
        AINFO << "Start " << uart_conf_->dev_setting().uart_dev() <<
            ", fd: " << serial_fd_ <<
            "ms, baud: " << uart_conf_->dev_setting().baud() << ", OK";
        return DriveDataItf::Start();
    }

    int SerialData::Stop() {
        AINFO << "Stop SerialData, dev: " <<
            uart_conf_->dev_setting().uart_dev();

        return DriveDataItf::Stop();
    }

    int SerialData::Resume() {
        AINFO << "Resume SerialData, dev: " <<
            uart_conf_->dev_setting().uart_dev();

        return DriveDataItf::Resume();
    }

    int SerialData::Close() {
#ifdef SERIAL_PORTING
        comm_->flush();
        comm_->closePort();
        delete comm_;
        comm_ = nullptr;
#else
        AINFO << "Close SerialData, dev: " <<
            uart_conf_->dev_setting().uart_dev();

        if (serial_fd_ > 0) {
            close_uart(serial_fd_);
            serial_fd_ = -1;
        } else {
            AWARN << uart_conf_->dev_setting().uart_dev() <<
                " not opened, can't be closed!";
        }
#endif
        DriveDataItf::Stop();
        return DriveDataItf::Close();
    }

    void SerialData::serial_poll_func() {
        int len = 0;
        uint8_t buf[uart_conf_->dev_setting().once_read_size()] = {0};
        if (serial_monitor_ == nullptr)
            AFATAL << "no uart listener";

        if ((len = read_uart(serial_fd_, buf,
                        uart_conf_->dev_setting().once_read_size())) > 0) {
#ifdef SERIAL_DBG
            std::ostringstream os;
            std::ostringstream oss;
            for(int i = 0; i < len; i++) {
                oss << std::hex << std::setw(2) << std::setfill('0') <<
                    static_cast<int>(buf[i]) << " ";
            }
            os.str("");
            os << std::dec << std::setw(3) << std::setfill('0') << len;
            AINFO << "get serial data, len: " <<
                os.str() << " <- " << uart_conf_->dev_setting().uart_dev() <<
                ", [ " << oss.str() << " ]";
#endif
            serial_monitor_((const void*)snsr_info_, buf, len);
        } else {
#ifdef SERIAL_DBG
            AWARN_EVERY(200) << "read dev: " << uart_conf_->dev_setting().uart_dev() <<
                ", len: " << len <<
                ", no data, ignore!!";
#endif
        }
    }

} //namespace driver
} //namespace chss
} //namespace mstf

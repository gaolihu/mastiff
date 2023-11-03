#pragma once

#include <atomic>
#include <functional>

#include "cyber/cyber.h"

namespace mstf {
namespace chss {
namespace driver {

//#define USE_CYBER_TIMER

    using DriveDataPolling = std::function<void()>;
    using DriveDataMonitor = std::function<void
        (const uint8_t*, const size_t)>;
    using DriveDataMonitorCan = std::function<void
        (const uint8_t*, const int)>;

    class DriveDataItf {
        public:
            DriveDataItf() = default;
            virtual ~DriveDataItf() = default;
            DriveDataItf(const DriveDataItf&) = delete;
            DriveDataItf& operator=(const DriveDataItf&) = delete;

            // generic operations
            virtual int Init(const int loop_cycle_ms, const
                    DriveDataPolling& p = nullptr) {
                if (p != nullptr) {
                    pollinger_ = p;
#ifdef USE_CYBER_TIMER
                    AINFO << "init cyber timer for getting low layer data!";
                    tim_.reset(new apollo::cyber::Timer(
                                loop_cycle_ms == 0 ? 20 : loop_cycle_ms,
                                [this] () {
                                    pollinger_();
                                },
                                false));
#else
                    AINFO << "init thread for getting low layer data!";
                    raw_capture_thread_ = std::thread(
                            [this] () {
                                while (!terminate_) {
                                    if (running_) {
                                        pollinger_();
                                    } else {
                                        std::this_thread::sleep_for(
                                            std::chrono::milliseconds(1000));
                                    }
                                }
                            });
#endif
                } else {
                    AWARN << "poller NULL!";
                }
                return 0;
            }
            virtual int Start() {
                AINFO << "capture thread start!";
#ifdef USE_CYBER_TIMER
                if (tim_)
                    tim_->Start();
                else
                    return -1;
#else
                running_ = true;
                terminate_ = false;
#endif
                return 0;
            }
            virtual int Stop() {
                AINFO << "capture thread stop!";
#ifdef USE_CYBER_TIMER
                if (tim_)
                    tim_->Stop();
                else
                    return -1;
#else
                running_ = false;
#endif
                return 0;
            }
            virtual int Close() {
                AINFO << "capture thread close!";
#ifdef USE_CYBER_TIMER
                if (tim_)
                    tim_.reset();
                else
                    return -1;
#else
                running_ = false;
                terminate_ = true;
                raw_capture_thread_.join();
#endif
                return 0;
            }

            // data status judge
            virtual const bool IsReadable() const {
                return readable_.load();
            };
            virtual void SetReadable(const bool b) {
                readable_.store(b);
            };
            virtual const bool IsWritable() const {
                return writable_.load();
            };
            virtual void SetWritable(const bool b) {
                writable_.store(b);
            };

            //once read config
            virtual const size_t GetOnceRead() const {
                return once_read_size_;
            };
            virtual void SetStatus(const size_t size) {
                once_read_size_ = size;
            };

            // status handles(get/set)
            virtual const std::string& GetStatus() const {
                return drive_data_status_;
            };
            virtual void SetStatus(const std::string& stat) {
                drive_data_status_ = stat;
            };

            // description handles(get/set)
            virtual const std::string& GetDriveDataDes() const {
                return drive_data_des_;
            };
            virtual void SetDriveDataDes(const std::string& des) {
                drive_data_des_ = des;
            };

            // message handles(receive/send)
            virtual int WritePort(const google::protobuf::Message&) {
                return 0;
            };
            virtual int WritePort(const std::vector<uint8_t>&) {
                return 0;
            };
            virtual int WritePort(const int, const uint8_t*, const size_t) {
                return 0;
            }
            virtual int RecvMessage(std::string&, int&) {
                return 0;
            };

        private:
            size_t once_read_size_ = 256;

            std::atomic<bool> readable_ = {false};
            std::atomic<bool> writable_ = {false};

            std::string drive_data_des_ = "";
            std::string drive_data_status_ = "";

            DriveDataPolling pollinger_ = nullptr;

            //timer common for data polling
#ifdef USE_CYBER_TIMER
            std::unique_ptr<apollo::cyber::Timer> tim_;
#else
            bool running_ = false;
            bool terminate_ = false;
            std::thread raw_capture_thread_;
#endif
    };

} // namespace driver
} // namespace chss
} // namespace mstf

#pragma once

#include "cyber/cyber.h"
#include "cyber/timer/timer.h"

#include "modules/chassis/proto/chassis_config.pb.h"

namespace mstf {
namespace chss {
namespace driver {

    using namespace /*mstf::chss::*/proto;
    using namespace google::protobuf;

    using DriveDataPolling = std::function<void()>;
    using DriveDataMonitor = std::function<void
        (const SensorIndicator*, const uint8_t*, const size_t)>;

    using UartDataListener = std::function<void
        (const SensorIndicator*, const uint8_t*, const size_t)>;

    using IIcDataListener = std::function<void
        (const SensorIndicator*, const uint8_t*, const size_t)>;

    using CanDataListener = std::function<void
        (const SensorIndicator*, const uint8_t*, const size_t)>;

    using JoyDataListener = std::function<void
        (const SensorIndicator*, const uint8_t*, const size_t)>;

    using SocDataListener = std::function<void
        (const SensorIndicator*, const Message&,
         const uint8_t*, const size_t)>;

    class DriveDataItf {
        public:
            DriveDataItf() = default;
            virtual ~DriveDataItf() = default;
            DriveDataItf(const DriveDataItf&) = delete;
            DriveDataItf& operator=(const DriveDataItf&) = delete;

            // generic operations
            virtual int Init(const std::string& dev, /*dev name*/
                    const int loop_cycle_ms) {       /*looping cycle*/
                if (!dev_.empty())
                    dev_ = dev;

                if (loop_cycle_ms <= 0)
                    loop_cycle_ms_ = 200;

#ifdef USE_CYBER_TIMER
                AINFO << "timer for <" <<
                    dev << "> getting low layer data!";
                tim_.reset(new apollo::cyber::Timer(
                            loop_cycle_ms_,
                            [this] () {
                                PollingDriveRutine();
                            },
                            false));
#else
                AINFO << "thread for <" <<
                    dev << "> getting low layer data!";
#endif
                return 0;
            }

            virtual int Start() {
                if (is_inited_) {
                    AWARN << "driver base already started!";
                    return -1;
                }
                AINFO << "capture " << dev_ <<
                    " thread " << loop_cycle_ms_ <<
                    "ms, start!";
#ifdef USE_CYBER_TIMER
                if (tim_)
                    tim_->Start();
                else
                    return -1;
#else
                running_ = true;
                terminate_ = false;
                is_inited_ = true;

                raw_capture_thread_ = std::thread(
                        [this] () {
                            while (!terminate_) {
                                if (running_) {
                                    PollingDriveRutine();
                                    std::this_thread::sleep_for(
                                            std::chrono
                                            ::milliseconds(loop_cycle_ms_ / 10));
                                } else {
                                    //AINFO << "stop & wait resume " << dev_;
                                    std::this_thread::sleep_for(
                                            std::chrono
                                            ::milliseconds(loop_cycle_ms_));
                                }
                            }
                            AWARN << "terminate & exit capture " << dev_;
                        });
#endif
                AINFO << "capture " << dev_ <<
                    " thread start OK!";

                return 0;
            }

            virtual int Stop() {
                AINFO << "capture " << dev_ <<
                    " thread stop!";
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

            virtual int Resume() {
                AINFO << "capture " << dev_ <<
                    " thread resume!";
#ifdef USE_CYBER_TIMER
                if (tim_)
                    tim_->Start();
                else
                    return -1;
#else
                running_ = true;
#endif
                return 0;
            }

            virtual int Close() {
                AINFO << "capture " << dev_ <<
                    " thread close!";
#ifdef USE_CYBER_TIMER
                if (tim_)
                    tim_.reset();
                else
                    return -1;
#else
                running_ = false;
                terminate_ = true;

                std::this_thread::sleep_for(std::chrono::
                        milliseconds(300));

                if (is_inited_) {
                    AWARN << "join thread for " << dev_;
                    raw_capture_thread_.join();
                    AWARN << "recycle thread OK!";
                    is_inited_ = false;
                }
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

            virtual void SetOnceRead(const size_t size) {
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
            virtual int WritePort(const Message&) {
                return 0;
            };

            virtual int WritePort(const std::vector<uint8_t>&) {
                return 0;
            };

            virtual int WritePort(const uint8_t*, const size_t) {
                return 0;
            }

            virtual int WritePort(const int, const uint8_t*, const size_t) {
                return 0;
            }

            virtual int RecvMessage(std::string&, int&) {
                return 0;
            };

            //pure ITFs
            virtual int Init(const SensorIndicator*) = 0;
            virtual int Start(const SensorIndicator*) = 0;
            virtual int Stop(const SensorIndicator*) = 0;
            virtual int Resume(const SensorIndicator*) = 0;
            virtual int Close(const SensorIndicator*) = 0;

            virtual void PollingDriveRutine() {
                AERROR << "Shall override this!";
            };

        protected:
            size_t once_read_size_ = 256;
            uint8_t* once_read_buf_ {};
            size_t loop_cycle_ms_ = 2048;   //blocking

            std::atomic<bool> readable_ { false };
            std::atomic<bool> writable_ { false };

            std::string dev_ { "DFT-DEV" };

            std::string drive_data_des_ {};
            std::string drive_data_status_ {};

            //timer common for data polling
#ifdef USE_CYBER_TIMER
            std::unique_ptr<apollo::cyber::Timer> tim_;
#else
            bool is_inited_ = false;
            bool running_ = false;
            bool terminate_ = false;
            std::thread raw_capture_thread_;
#endif
            //dev fd
            int dev_fd_ = -1;
            const SensorIndicator* s_idc_ {};

            DriveDataMonitor data_monitor_ {};
    };

} // namespace driver
} // namespace chss
} // namespace mstf

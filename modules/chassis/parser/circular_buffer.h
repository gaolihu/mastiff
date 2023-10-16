#pragma once

#include <mutex>
#include <iomanip>
#include <vector>
#include <condition_variable>
//#include <boost/noncopyable.hpp>

#include "cyber/common/log.h"

/*
 * Author: Glh
 * Date: 2023/9/15 11:33
 *
 * WARNING: DON'T CHANGE THIS FILE!
 */

//#define CBUF_DBG 1

namespace mstf {
namespace chss {
namespace parser {

    //using namespace std::chrono_literals;
    template <class T>
    //class CircularBuffer : public boost::noncopyable {
    class CircularBuffer {
        public:
            CircularBuffer() {}
            CircularBuffer(CircularBuffer&) = delete;
            virtual ~CircularBuffer() {
                Close();
                data_.resize(0);
            }

            explicit CircularBuffer(size_t s,
                    const std::string dev = "") {
                //data_.reserve(s);
                data_.resize(s);
                capacity_all_ = s;
                capacity_free_ = s;
                head_ = 0;
                tail_ = 0;
                dev_ = dev;
            }

            bool Closed() const {
#ifdef CBUF_DBG
                ShowCbuf();
#endif
                return closed_;
            }
            void Open() {
#ifdef CBUF_DBG
                AINFO << "Open circular buffer: " <<
                    dev_;
                ShowCbuf();
#endif
                LockGuard lg(mutex_);
                if (closed_) {
                    closed_ = false;
                }
                AINFO << "open cbuf for: " << dev_ <<
                    ", size: " << data_.size();
            }
            void Close()  {
#ifdef CBUF_DBG
                AINFO << "Close circular buffer: " <<
                    dev_;
                ShowCbuf();
#endif
                LockGuard lg(mutex_);
                closed_ = true;
                read_cond_var_.notify_one();
                write_cond_var_.notify_one();
            }
            void SetCapacity(size_t s) {
                LockGuard lg(mutex_);
                //clear data
                data_.resize(s);
                capacity_all_ = s;
                capacity_free_ = s;
                head_ = 0;
                tail_ = 0;
            }
            bool Empty() const {
                LockGuard lg(mutex_);
                return capacity_free_ == capacity_all_;
            }
            bool Full() const {
                LockGuard lg(mutex_);
                return capacity_free_ == 0;
            }
            bool Push(const T&& val) {
                //push single element
                if (closed_) {
                    AWARN << "cbuf closed!";
                    return false;
                }
                UniqueLock lg(mutex_);
                if (capacity_free_ < 1) {
                    //no space for writing
                    write_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(200),
                        [this]()->bool {
                            bool writable = this->capacity_free_ >= 1;
                            if (!writable) {
                                AWARN << "!!waiting for writable, at least 1, " <<
                                    ", dev: " << dev_ <<
                                    ", compare free space: " << this->capacity_free_;
                                ShowCbuf();
                                read_cond_var_.notify_all();
                            }
                            return writable;
                        }
                    );
                }
                if (!WriteCore(&val, 1))
                    return false;
#ifdef CBUF_DBG
                AINFO << "push single";
                ShowCbuf();
#endif
                lg.unlock();
                read_cond_var_.notify_one();
                return true;
            }
            bool Push(const T* val, int len) {
                //push len elements
                if (closed_) {
                    AWARN << "cbuf closed!";
                    return false;
                }
                // UniqueLock lg(mutex_);
                if (capacity_free_ < len) {
                    // no space for writing
                    RestoreCbuf(len - capacity_free_);
                    // write_cond_var_.wait_for(lg,
                    //     std::chrono::milliseconds(200),
                    //     [this, len]()->bool {
                    //         bool writable = this->capacity_free_ >= len;
                    //         if (!writable) {
                    //             AWARN << "!!waiting for writable, at least: " << len <<
                    //                 ", dev: " << dev_ <<
                    //                 ", compare free space: " << this->capacity_free_;
                    //             ShowCbuf();
                    //             read_cond_var_.notify_all();
                    //         }
                    //         return writable;
                    //     }
                    // );
                }
                if (!WriteCore(val, len))
                    return false;
#ifdef CBUF_DBG
                AINFO << "push len: " << len;
                ShowCbuf();
#endif
                // lg.unlock();
                read_cond_var_.notify_one();
                return true;
            }
            bool Read(std::vector<T>& p) {
                //read all elements
                if (closed_)
                    return true;
                UniqueLock lg(mutex_);
                if ((capacity_all_ - capacity_free_) == 0) {
                    //|| (data_.size() <= 0)
                    //no data for reading
                    read_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(80),
                        [&]()->bool {
                            bool readable = (capacity_all_ - capacity_free_) > 0;
                            if (!readable) {
                                AWARN << "!!waiting for readable at least 1, " <<
                                    ", dev: " << dev_ <<
                                    ", compare have: " << capacity_all_ - capacity_free_;
                                ShowCbuf();
                                write_cond_var_.notify_all();
                            }
                            return readable;
                        }
                    );
                }
                if (!ReadCore(p, capacity_all_ - capacity_free_))
                    return false;
                tail_ = head_;
                capacity_free_ = capacity_all_;
                lg.unlock();
                write_cond_var_.notify_one();
                return true;
            }
            bool Read(std::vector<T>& p, int len) {
                if (closed_)
                    return true;
                //read len elements
                UniqueLock lg(mutex_);
                if ((capacity_all_ - capacity_free_) < len) {
                    //|| (data_.size() <= 0)
                    //no data for reading
                    read_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(80),
                        [&]()->bool {
                            bool readable = (capacity_all_ - capacity_free_) >= len;
                            if (!readable) {
                                AWARN << "!!waiting for readable, len: " << len <<
                                    ", dev: " << dev_ <<
                                    ", compare have: " << capacity_all_ - capacity_free_;
                                ShowCbuf();
                                write_cond_var_.notify_all();
                            }
                            return readable;
                        }
                    );
                }
                if (!ReadCore(p, len))
                    return false;
                tail_ = (tail_ + len) % capacity_all_;
                capacity_free_ += len;
                lg.unlock();
                write_cond_var_.notify_one();
                return true;
            }
            bool ReadSafely(std::vector<T>& p) {
                //read all elements safelly
                if (closed_)
                    return true;
                UniqueLock lg(mutex_);
                if ((capacity_all_ - capacity_free_) == 0) {
                    //|| (data_.size() <= 0)
                    AINFO << "no data for reading" <<
                        ", dev: " << dev_;
                    read_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(80),
                        [&]()->bool {
                            bool readable = (capacity_all_ - capacity_free_) >= 0;
                            if (!readable) {
                                AWARN << "!!waiting for readable, at least 1, " <<
                                    ", dev: " << dev_ <<
                                    ", compare have: " << capacity_all_ - capacity_free_;
                                ShowCbuf();
                                write_cond_var_.notify_all();
                            }
                            return readable;
                        }
                    );
                }
                if (!ReadCore(p, capacity_all_ - capacity_free_)) {
                    AWARN << "read: " << dev_ <<
                        ", core error, size: " << capacity_all_ - capacity_free_;
                    lg.unlock();
                    write_cond_var_.notify_all();
                    return false;
                }
                lg.unlock();
                write_cond_var_.notify_one();
                return true;
            }
            bool ReadSafely(std::vector<T>& p, int len) {
                //read len elements safelly
                if (closed_)
                    return true;
                UniqueLock lg(mutex_);
                if ((capacity_all_ - capacity_free_) < len) {
                    //|| (data_.size() <= 0)
                    AINFO << "no data for reading" <<
                        ", dev: " << dev_;
                    read_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(80),
                        [&]()->bool {
                            bool readable = (capacity_all_ - capacity_free_) >= len;
                            if (!readable) {
                                AWARN << "!!waiting for readable, len: " << len <<
                                    ", dev: " << dev_ <<
                                    ", compare have: " << capacity_all_ - capacity_free_;
                                ShowCbuf();
                                write_cond_var_.notify_all();
                            }
                            return readable;
                        }
                    );
                }
                if (ReadCore(p, len))
                    return false;
                lg.unlock();
                write_cond_var_.notify_one();
                return true;
            }
            bool RestoreCbuf() {
                return RestoreCbuf(capacity_all_ - capacity_free_);
            }
            bool RestoreCbuf(int len) {
                LockGuard lg(mutex_);
                if (len == capacity_all_ - capacity_free_ + 1) {
                    //first CT crc byte for eai lidar
                    tail_ = head_ = 0;
                    capacity_free_ = capacity_all_;
                    write_cond_var_.notify_all();
                    return true;
                } else if (len > (capacity_all_ - capacity_free_)) {
                    ShowCbuf("Restore error", len);
                    AWARN << "Restore: " << len <<
                        ", more than circular possess?!" <<
                        capacity_all_ - capacity_free_;
                    tail_ = head_ = 0;
                    capacity_free_ = capacity_all_;
                    write_cond_var_.notify_all();
                    return false;
                }
                tail_ = (tail_ + len) % capacity_all_;
                capacity_free_ += len;
                write_cond_var_.notify_all();
#ifdef CBUF_DBG
                ShowCbuf("after free", len);
#endif
                return true;
            }
            bool CheckCbuf() {
#ifdef CBUF_DBG
                ShowCbuf("show cbuf before check");
#endif
                check_cnt_++;

                if (statistic_len_ == statistic_len_last_) {
                    //buf unchanged
                    statistic_len_last_ = statistic_len_;
                    return true;
                } else {
                    //buf changed
                    statistic_len_last_ = statistic_len_;
                    return false;
                }
            }
            void ReleaseCbuf() {
#ifdef CBUF_DBG
                AWARN << "release cbuf: " << dev_ <<
                    ", check count: " << check_cnt_ <<
                    ", statistic: " << statistic_len_ <<
                    ", statistic last: " << statistic_len_last_;
#endif
                read_cond_var_.notify_one();
                write_cond_var_.notify_one();
            }
            int CbufSize() {
                LockGuard lg(mutex_);
                return capacity_all_ - capacity_free_;
            }

        private:
            bool ReadCore(std::vector<T>& p, int len) {
                //read len elements
                //AINFO << "len:" << len << " vector len:" << p.size();
                if (closed_)
                    return true;
                //
                ACHECK(((int)data_.capacity() >= len) ||
                        (capacity_all_ >= len)) <<
                    "read len error!";
#ifdef CBUF_DBG
                std::ostringstream os;
#endif
                //AINFO << "head:" << head_ << " tail_:" << tail_;
                if (tail_ < head_) {
                    for (int i = tail_; i < tail_ + len; i++) {
#ifdef CBUF_DBG
                        os << std::hex << std::setw(2) << std::setfill('0') <<
                            static_cast<int>(data_[i]) << " ";
#endif
                        p[i - tail_] = data_[i];
                    }
                } else {
                    int cursor = 0, j = 0;
                    if (tail_ + len <= capacity_all_) {
                        for (j = tail_; j < tail_ + len; j++, cursor++) {
#ifdef CBUF_DBG
                            os << std::hex << std::setw(2) << std::setfill('0') <<
                                static_cast<int>(data_[j]) << " ";
#endif
                            p[cursor] = data_[j];
                        }
                    } else {
                        for (j = tail_; j < capacity_all_; j++, cursor++) {
#ifdef CBUF_DBG
                            os << std::hex << std::setw(2) << std::setfill('0') <<
                                static_cast<int>(data_[j]) << " ";
#endif
                            //AINFO << "cursor:" << cursor << " j:" << j;
                            p[cursor] = data_[j];
                        }
                        for (j = 0; j < len - capacity_all_ + tail_; j++, cursor++) {
#ifdef CBUF_DBG
                            os << std::hex << std::setw(2) << std::setfill('0') <<
                                static_cast<int>(data_[j]) << " ";
#endif
                            //AINFO << "cursor:" << cursor << " j:" << j;
                            p[cursor] = data_[j];//cursor more than max size
                        }
                    }
                }
#ifdef CBUF_DBG
                AINFO << " POP len: " << len <<
                    ", dev: " << dev_ <<
                    ", Cbuf contents: [ " << os.str() << " ]";
                ShowCbuf();
#endif
                return true;
            }
            bool WriteCore(const T* val, int len) {
                //push len elements
                if (closed_)
                    return true;
                ACHECK((len > 0) && (len <= capacity_all_)) <<
                    "write len error, len: " << len <<
                    ", capacity_all_: " << capacity_all_;

                statistic_len_ += len;
#ifdef CBUF_DBG
                std::ostringstream os;
#endif
                for (int i = 0; i < len; i++) {
                    data_[head_] = val[i];
#ifdef CBUF_RAW
                    os << std::hex << std::setw(2) << std::setfill('0') <<
                        static_cast<int>(data_[head_]) << " ";
#endif
                    head_ = (head_ + 1) % capacity_all_;
                    capacity_free_--;
                }
#ifdef CBUF_DBG
                //
                AINFO << "PUSH len: " << len <<
                    ", dev: " << dev_ <<
                    ", Cbuf contents: [ " << os.str() << " ]";
                ShowCbuf();
#endif
                return true;
            }
            void ShowCbuf(const char* p = nullptr, const int x = -1) {
                std::ostringstream os;
                std::ostringstream oss;
#ifdef CBUF_RAW
                os << std::hex << std::setw(2) << std::setfill('0') <<
                    "{ " <<
                    static_cast<int>(data_[tail_]) << "/" <<
                    static_cast<int>(data_[tail_ + 1]) << "/" <<
                    static_cast<int>(data_[tail_ + 2]) << "/" <<
                    static_cast<int>(data_[tail_ + 3]) << "/" <<
                    static_cast<int>(data_[tail_ + 4]) << "/" <<
                    static_cast<int>(data_[tail_ + 5]) << "/" <<
                    static_cast<int>(data_[tail_ + 6]) << "/" <<
                    static_cast<int>(data_[tail_ + 7]) << "/" <<
                    static_cast<int>(data_[tail_ + 8]) << "/" <<
                    static_cast<int>(data_[tail_ + 9]) << "/" <<
                    static_cast<int>(data_[tail_ + 10]) << "/" <<
                    static_cast<int>(data_[tail_ + 11]) << " }";
#endif
                if (p != nullptr) {
                    oss << "[";
                    oss << p;
                    oss << "]";
                    if (x != -1)
                        oss << ", Num: " << x << ", ";
                }
                AINFO << oss.str() <<
                    "  cbuf: " << dev_ <<
                    ", capacity: " << capacity_all_ <<
                    ", free: " << capacity_free_ <<
                    ", head: " << head_ <<
                    ", tail: " << tail_ <<
                    ", tail to head: " << head_ - tail_ <<
#ifdef CBUF_RAW
                    ", data_[tail ~ tail + 11]: " << os.str() <<
#endif
                    ", used: " << capacity_all_ - capacity_free_ <<
                    ", cbuf size: " << data_.size() <<
                    ", cbuf cap: " << data_.size();
            }

        private:
            typedef std::unique_lock<std::mutex> UniqueLock;
            typedef std::lock_guard<std::mutex> LockGuard;
            std::condition_variable read_cond_var_;
            std::condition_variable write_cond_var_;
            mutable std::mutex mutex_;
            std::vector<T> data_;
            std::string dev_;

            bool closed_ = false;
            int head_ = -1;
            int tail_ = -1;
            int capacity_all_ = 0;
            int capacity_free_ = 0;

            size_t statistic_len_ = 0;
            size_t statistic_len_last_ = 0;
            int check_cnt_ = 0;
    };

} //namespace parser
} //namespace chss
} //namespace mstf

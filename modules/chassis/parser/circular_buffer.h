#pragma once

#include <mutex>
#include <iomanip>
#include <vector>
#include <condition_variable>
//#include <boost/noncopyable.hpp>

#include "cyber/common/log.h"

/*
 * Author: Glh
 *
 * Create: 2023/09/15 11:33
 * Review: 2023/11/13 10:10
 *
 * WARNING: DON'T MODIFY THIS FILE!!!
 */

//#define CBUF_DBG 1
//#define CBUF_RAW 1

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
            CircularBuffer(const CircularBuffer&) = delete;
            CircularBuffer& operator=(const CircularBuffer&) = delete;

            virtual ~CircularBuffer() {
                Close();
                data_.resize(0);
                data_.clear();
            }

            explicit CircularBuffer(size_t s,
                    const std::string dev = "DFT") {
                AINFO << "create cbuf: " << dev <<
                    ", size: " << s;
                //data_.reserve(s);
                data_.resize(s);
                capacity_cbuf_ = s;
                capacity_free_ = s;
                head_ = 0;
                tail_ = 0;
                buf_name_ = dev;
            }

            void Open() {
                ShowCbuf("open");

                LockGuard lg(mutex_);
                if (is_closed_) {
                    is_closed_ = false;
                }
                AINFO << "open cbuf for " << buf_name_ <<
                    ", capacity: " << data_.size();
            }

            void Close()  {
                ShowCbuf("close");

                LockGuard lg(mutex_);
                if (!is_closed_)
                    is_closed_ = true;
                read_cond_var_.notify_one();
                write_cond_var_.notify_one();
            }

            void SetCapacity(size_t s) {
                LockGuard lg(mutex_);
                ShowCbuf("set cap");

                data_.resize(s);
                capacity_cbuf_ = s;
                capacity_free_ = s;
                head_ = 0;
                tail_ = 0;
            }

            bool IsEmpty() const {
                LockGuard lg(mutex_);
                ShowCbuf("is empty");

                return capacity_free_ == capacity_cbuf_;
            }

            bool IsFull() const {
                LockGuard lg(mutex_);
                ShowCbuf("is full");

                return capacity_free_ == 0;
            }

            bool IsClosed() const {
                LockGuard lg(mutex_);
                ShowCbuf("is close");

                return is_closed_;
            }

            bool Push(const T&& val) {
                //push single element
                if (is_closed_) {
                    AWARN << "cbuf " << buf_name_ <<
                        " already closed, reject push!";
                    return false;
                }
                UniqueLock lg(mutex_);
                if (capacity_free_ < 1) {
                    //no space for writing
                    write_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(20),
                        [this]()->bool {
                            bool writable = this->capacity_free_ >= 1;
                            if (!writable) {
                                AWARN << "!!waiting for writable, at least 1, " <<
                                    ", dev: " << buf_name_ <<
                                    ", compare free space: " << this->capacity_free_;
                                ShowCbuf("push single wrong");
                                read_cond_var_.notify_all();
                            }
                            return writable;
                        }
                    );
                }
                if (!WriteCore(&val, 1))
                    return false;
#ifdef CBUF_DBG
                ShowCbuf("push single");
#endif
                lg.unlock();
                read_cond_var_.notify_one();
                return true;
            }

            bool Push(const T* val, int len) {
                //push len elements
                if (is_closed_) {
                    AWARN << "cbuf " << buf_name_ <<
                        " already closed, reject push!";
                    return false;
                }
                UniqueLock lg(mutex_);
                if (capacity_free_ < len) {
                    // no space for writing
                    AWARN << "cbuf space not enough!!!";
                    Restore(len - capacity_free_);
                    if (!write_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(20),
                        [this, len]()->bool {
                            bool writable = this->capacity_free_ >= len;
                            if (!writable) {
                                AWARN << "!!waiting for writable, at least: " << len <<
                                    ", dev: " << buf_name_ <<
                                    ", compare free space: " << this->capacity_free_;
                                ShowCbuf("push wrong");
                                read_cond_var_.notify_all();
                            }
                            return writable;
                        }
                    )) {
                        AWARN << "Push len: " << len <<
                            "timeout!";
                    }
                }
                if (!WriteCore(val, len))
                    return false;
                lg.unlock();
                read_cond_var_.notify_one();
                return true;
            }

            bool Read(std::vector<T>& p) {
                //read all elements
                if (is_closed_) {
                    AWARN << "cbuf " << buf_name_ <<
                        " already closed, reject push!";
                    return true;
                }
                UniqueLock lg(mutex_);
                if ((capacity_cbuf_ - capacity_free_) == 0) {
                    //|| (data_.size() <= 0)
                    //no data for reading
                    read_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(20),
                        [&]()->bool {
                            bool readable = (capacity_cbuf_ - capacity_free_) > 0;
                            if (!readable) {
                                AWARN << "!!waiting for readable at least 1, " <<
                                    ", dev: " << buf_name_ <<
                                    ", compare have: " << capacity_cbuf_ - capacity_free_;
                                ShowCbuf("read all wrong");
                                write_cond_var_.notify_all();
                            }
                            return readable;
                        }
                    );
                }
                if (!ReadCore(p, capacity_cbuf_ - capacity_free_))
                    return false;
                tail_ = head_;
                capacity_free_ = capacity_cbuf_;
                lg.unlock();
                write_cond_var_.notify_one();
                return true;
            }

            bool Read(std::vector<T>& p, int len) {
                if (is_closed_) {
                    AWARN << "cbuf " << buf_name_ <<
                        " already closed, reject push!";
                    return true;
                }
                //read len elements
                UniqueLock lg(mutex_);
                if ((capacity_cbuf_ - capacity_free_) < len) {
                    //|| (data_.size() <= 0)
                    //no data for reading
                    read_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(20),
                        [&]()->bool {
                            bool readable = (capacity_cbuf_ - capacity_free_) >= len;
                            if (!readable) {
                                AWARN << "!!waiting for readable, len: " << len <<
                                    ", dev: " << buf_name_ <<
                                    ", compare have: " << capacity_cbuf_ - capacity_free_;
                                ShowCbuf("read wrong");
                                write_cond_var_.notify_all();
                            }
                            return readable;
                        }
                    );
                }
                if (!ReadCore(p, len))
                    return false;
                tail_ = (tail_ + len) % capacity_cbuf_;
                capacity_free_ += len;
                lg.unlock();
                write_cond_var_.notify_one();
                return true;
            }

            bool ReadSafely(std::vector<T>& p) {
                //read all elements safelly
                if (is_closed_) {
                    AWARN << "cbuf " << buf_name_ <<
                        " already closed, reject push!";
                    return true;
                }
                UniqueLock lg(mutex_);
                if ((capacity_cbuf_ - capacity_free_) == 0) {
                    //|| (data_.size() <= 0)
                    AINFO << "no data for reading" <<
                        ", dev: " << buf_name_;
                    read_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(20),
                        [&]()->bool {
                            bool readable = (capacity_cbuf_ - capacity_free_) >= 0;
                            if (!readable) {
                                AWARN << "!!waiting for readable, at least 1, " <<
                                    ", dev: " << buf_name_ <<
                                    ", compare have: " << capacity_cbuf_ - capacity_free_;
                                ShowCbuf("read all safely wrong");
                                write_cond_var_.notify_all();
                            }
                            return readable;
                        }
                    );
                }
                if (!ReadCore(p, capacity_cbuf_ - capacity_free_)) {
                    AWARN << "read: " << buf_name_ <<
                        ", core error, size: " << capacity_cbuf_ - capacity_free_;
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
                if (is_closed_) {
                    AWARN << "cbuf " << buf_name_ <<
                        " already closed, reject push!";
                    return true;
                }
                UniqueLock lg(mutex_);
                if ((capacity_cbuf_ - capacity_free_) < len) {
                    //|| (data_.size() <= 0)
                    AINFO << "no data for reading" <<
                        ", dev: " << buf_name_;
                    read_cond_var_.wait_for(lg,
                        std::chrono::milliseconds(20),
                        [&]()->bool {
                            bool readable = (capacity_cbuf_ - capacity_free_) >= len;
                            if (!readable) {
                                AWARN << "!!waiting for readable, len: " << len <<
                                    ", dev: " << buf_name_ <<
                                    ", compare have: " << capacity_cbuf_ - capacity_free_;
                                ShowCbuf("read safely wrong");
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

            //release all buffer
            bool Restore() {
                return Restore(capacity_cbuf_ - capacity_free_);
            }

            bool Restore(int len) {
                LockGuard lg(mutex_);
                if (len == capacity_cbuf_ - capacity_free_ + 1) {
                    tail_ = head_ = 0;
                    capacity_free_ = capacity_cbuf_;
                    write_cond_var_.notify_all();
                    return true;
                } else if (len > (capacity_cbuf_ - capacity_free_)) {
                    ShowCbuf("Restore error", len);
                    AWARN << "Restore: " << len <<
                        ", more than circular possess?!" <<
                        capacity_cbuf_ - capacity_free_;
                    tail_ = head_ = 0;
                    capacity_free_ = capacity_cbuf_;
                    write_cond_var_.notify_all();
                    return false;
                }
                tail_ = (tail_ + len) % capacity_cbuf_;
                capacity_free_ += len;
                write_cond_var_.notify_all();
#ifdef CBUF_DBG
                ShowCbuf("restore after", len);
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
                AWARN << "release cbuf: " << buf_name_ <<
                    ", check count: " << check_cnt_ <<
                    ", statistic: " << statistic_len_ <<
                    ", statistic last: " << statistic_len_last_;
#endif
                read_cond_var_.notify_one();
                write_cond_var_.notify_one();
            }

            int CbufSize() {
                LockGuard lg(mutex_);
                return capacity_cbuf_ - capacity_free_;
            }

        private:
            bool ReadCore(std::vector<T>& p, int len) {
                //read len elements
                //AINFO << "len:" << len << " vector len:" << p.size();
                if (is_closed_)
                    return true;
                //
                ACHECK(((int)data_.capacity() >= len) ||
                        (capacity_cbuf_ >= len)) <<
                    "read " << buf_name_ <<
                    " len: " << len <<
                    ", capacity: " << data_.capacity() <<
                    ", all: " << capacity_cbuf_;
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
                    if (tail_ + len <= capacity_cbuf_) {
                        for (j = tail_; j < tail_ + len; j++, cursor++) {
#ifdef CBUF_DBG
                            os << std::hex << std::setw(2) << std::setfill('0') <<
                                static_cast<int>(data_[j]) << " ";
#endif
                            p[cursor] = data_[j];
                        }
                    } else {
                        for (j = tail_; j < capacity_cbuf_; j++, cursor++) {
#ifdef CBUF_DBG
                            os << std::hex << std::setw(2) << std::setfill('0') <<
                                static_cast<int>(data_[j]) << " ";
#endif
                            //AINFO << "cursor:" << cursor << " j:" << j;
                            p[cursor] = data_[j];
                        }
                        for (j = 0; j < len - capacity_cbuf_ + tail_; j++, cursor++) {
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
                std::ostringstream oss;
                oss << "Core POP: " << len <<
                    ", Cbuf contents: [ "
#ifdef CBUF_RAW
                    << os.str() <<
#endif
                    " ]";
                ShowCbuf(oss.str().data());
#endif
                return true;
            }

            bool WriteCore(const T* val, int len) {
                //push len elements
                if (is_closed_)
                    return true;
                ACHECK((len > 0) && (len <= capacity_cbuf_)) <<
                    "write len error, len: " << len <<
                    ", capacity_cbuf_: " << capacity_cbuf_;

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
                    head_ = (head_ + 1) % capacity_cbuf_;
                    capacity_free_--;
                }
#ifdef CBUF_DBG
                std::ostringstream oss;
                oss << "Core WRITE: " << len <<
                    ", Cbuf contents: [ " << os.str() << " ]";
                ShowCbuf(oss.str().data());
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
                        oss << " free: " << x << ", ";
                }
                AINFO << oss.str() <<
                    " CBUF: " << buf_name_ <<
                    ", capacity: " << capacity_cbuf_ <<
                    ", head: " << head_ <<
                    ", tail: " << tail_ <<
                    " ~  " << head_ - tail_ <<
#ifdef CBUF_RAW
                    ", data_[tail ~ tail + 11]: " << os.str() <<
#endif
                    ", used: " << capacity_cbuf_ - capacity_free_ <<
                    ", free: " << capacity_free_ <<
                    ", buf size: " << data_.size() <<
                    ", cap: " << data_.size();
            }

        private:
            typedef std::unique_lock<std::mutex> UniqueLock;
            typedef std::lock_guard<std::mutex> LockGuard;
            std::condition_variable read_cond_var_;
            std::condition_variable write_cond_var_;
            mutable std::mutex mutex_;
            std::vector<T> data_;
            std::string buf_name_;

            bool is_closed_ = true;
            int head_ = -1;
            int tail_ = -1;
            int capacity_cbuf_ = 0;
            int capacity_free_ = 0;

            size_t statistic_len_ = 0;
            size_t statistic_len_last_ = 0;
            int check_cnt_ = 0;
    };

} //namespace parser
} //namespace chss
} //namespace mstf

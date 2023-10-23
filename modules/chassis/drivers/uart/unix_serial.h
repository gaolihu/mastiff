#if !defined(_WIN32)

#ifndef SERIAL_IMPL_UNIX_H
#define SERIAL_IMPL_UNIX_H

#include <pthread.h>
#include <assert.h>
#include <termios.h>

namespace mstf {
namespace chss {
namespace driver {

#define DEFAULT_TIMEOUT 2000

    using std::size_t;
    using std::string;

    /*!
     * Enumeration defines the possible bytesizes for the serial port.
     */
    typedef enum {
        fivebits = 5,
        sixbits = 6,
        sevenbits = 7,
        eightbits = 8
    } bytesize_t;

    /*!
     * Enumeration defines the possible parity types for the serial port.
     */
    typedef enum {
        parity_none = 0,
        parity_odd = 1,
        parity_even = 2,
        parity_mark = 3,
        parity_space = 4
    } parity_t;

    /*!
     * Enumeration defines the possible stopbit types for the serial port.
     */
    typedef enum {
        stopbits_one = 1,
        stopbits_two = 2,
        stopbits_one_point_five
    } stopbits_t;

    /*!
     * Enumeration defines the possible flowcontrol types for the serial port.
     */
    typedef enum {
        flowcontrol_none = 0,
        flowcontrol_software,
        flowcontrol_hardware
    } flowcontrol_t;

    /// Defines all error codes handled by the CSimpleSocket class.
    typedef enum {
        NoError,
        DeviceNotFoundError,
        PermissionError,
        OpenError,
        ParityError,
        FramingError,
        BreakConditionError,
        WriteError,
        ReadError,
        ResourceError,
        UnsupportedOperationError,
        UnknownError,
        TimeoutError,
        NotOpenError
    } SerialPortError;

    /*!
     * Structure for setting the timeout of the serial port, times are
     * in milliseconds.
     *
     * In order to disable the interbyte timeout, set it to Timeout::max().
     */
    struct Timeout {
#ifdef max
# undef max
#endif
        static uint32_t max() {
            return std::numeric_limits<uint32_t>::max();
        }
        /*!
         * Convenience function to generate Timeout structs using a
         * single absolute timeout.
         *
         * \param timeout A long that defines the time in milliseconds until a
         * timeout occurs after a call to read or write is made.
         *
         * \return Timeout struct that represents this simple timeout provided.
         */
        static Timeout simpleTimeout(uint32_t timeout) {
            return Timeout(max(), timeout, 0, timeout, 0);
        }

        /*! Number of milliseconds between bytes received to timeout on. */
        uint32_t inter_byte_timeout;
        /*! A constant number of milliseconds to wait after calling read. */
        uint32_t read_timeout_constant;
        /*! A multiplier against the number of requested bytes to wait after
         *  calling read.
         */
        uint32_t read_timeout_multiplier;
        /*! A constant number of milliseconds to wait after calling write. */
        uint32_t write_timeout_constant;
        /*! A multiplier against the number of requested bytes to wait after
         *  calling write.
         */
        uint32_t write_timeout_multiplier;

        explicit Timeout(uint32_t inter_byte_timeout_ = 0,
                uint32_t read_timeout_constant_ = 0,
                uint32_t read_timeout_multiplier_ = 0,
                uint32_t write_timeout_constant_ = 0,
                uint32_t write_timeout_multiplier_ = 0)
            : inter_byte_timeout(inter_byte_timeout_),
            read_timeout_constant(read_timeout_constant_),
            read_timeout_multiplier(read_timeout_multiplier_),
            write_timeout_constant(write_timeout_constant_),
            write_timeout_multiplier(write_timeout_multiplier_) {
            }
    };

    class MillisecondTimer {
        public:
            explicit MillisecondTimer(const uint32_t millis);
            int64_t remaining();

        private:
            static timespec timespec_now();
            timespec expiry;
    };

    class SerialImpl {
        public:

            explicit SerialImpl(const string &port,
                    unsigned long baudrate,
                    bytesize_t bytesize,
                    parity_t parity,
                    stopbits_t stopbits,
                    flowcontrol_t flowcontrol);

            virtual ~SerialImpl();

            bool open();

            SerialPortError getSystemError(int systemErrorCode) const;

            void close();

            bool isOpen() const;

            size_t available();

            bool waitReadable(uint32_t timeout);

            void waitByteTimes(size_t count);

            int waitfordata(size_t data_count, uint32_t timeout, size_t *returned_size);

            size_t read(uint8_t *buf, size_t size = 1);

            size_t write(const uint8_t *data, size_t length);


            void flush();

            void flushInput();

            void flushOutput();

            void sendBreak(int duration);

            bool setBreak(bool level);

            bool setRTS(bool level);

            bool setDTR(bool level);

            bool waitForChange();

            bool getCTS();

            bool getDSR();

            bool getRI();

            bool getCD();

            uint32_t getByteTime();

            void setPort(const string &port);

            string getPort() const;

            void setTimeout(Timeout &timeout);

            Timeout getTimeout() const;

            bool setBaudrate(unsigned long baudrate);

            bool setStandardBaudRate(speed_t baudrate);

            bool setCustomBaudRate(unsigned long baudrate);

            unsigned long getBaudrate() const;

            bool setBytesize(bytesize_t bytesize);

            bytesize_t getBytesize() const;

            bool setParity(parity_t parity);

            parity_t getParity() const;

            bool setStopbits(stopbits_t stopbits);

            stopbits_t getStopbits() const;

            bool setFlowcontrol(flowcontrol_t flowcontrol);

            flowcontrol_t getFlowcontrol() const;

            bool setTermios(const termios *tio);

            bool getTermios(termios *tio);

            int readLock();

            int readUnlock();

            int writeLock();

            int writeUnlock();


        private:
            string port_;               // Path to the file descriptor
            int fd_;                    // The current file descriptor
            pid_t pid;

            bool is_open_;
            bool xonxoff_;
            bool rtscts_;

            Timeout timeout_;           // Timeout for read operations
            unsigned long baudrate_;    // Baudrate
            uint32_t byte_time_ns_;     // Nanoseconds to transmit/receive a single byte

            parity_t parity_;           // Parity
            bytesize_t bytesize_;       // Size of the bytes
            stopbits_t stopbits_;       // Stop Bits
            flowcontrol_t flowcontrol_; // Flow Control

            // Mutex used to lock the read functions
            pthread_mutex_t read_mutex;
            // Mutex used to lock the write functions
            pthread_mutex_t write_mutex;
    };

} //namespace driver
} //namespace chss
} //namespace mstf

#endif // SERIAL_IMPL_UNIX_H

#endif // !defined(_WIN32)

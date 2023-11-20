#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/epoll.h>

#include "uart_drive.h"

static int fd_epoll_ = -1;
static int poll_time_out_ = 0;

static int set_uart_speed(int fd, int speed)
{
    int i;
    int status = 0;
    struct termios opt;
    const tcflag_t baud_array[] = {
        B0, B50, B75, B110, B134, B150, B200, B300, B600,
        B1200, B1800, B2400, B4800, B9600, B19200, B38400,
        B57600, B115200, B230400, B460800, B500000, B576000,
        B921600, B1000000, B1152000, B1500000, B2000000, B2500000,
        B3000000, B3500000, B4000000
    };

    const speed_t speed_array[] = {
        0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800,
        9600, 19200, 38400, 57600, 115200, 230400, 460800,
        500000, 576000, 921600, 1000000, 1152000, 1500000, 2000000,
        2500000, 3000000, 3500000, 4000000
    };

    tcgetattr(fd, &opt);

    for (i = 0; i < (int)sizeof(baud_array) / (int)sizeof(int); i++) {
        if(speed == (int)speed_array[i]) {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&opt, baud_array[i]);
            cfsetospeed(&opt, baud_array[i]);
            status = tcsetattr(fd, TCSANOW, &opt);

            if(status == 0)
                tcflush(fd, TCIOFLUSH);

            break;
        }
    }

    return status;
}

static int set_uart_ctrl_flag(int fd, int databits, int stopbits, int parity)
{
    int result;
    struct termios options;

    result = tcgetattr(fd, &options);
    if(result != 0)
        return result;

    options.c_cflag &= ~CSIZE; /* Mask the character size bits */

    switch (databits) {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        break;
    }

    switch (parity) {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB; /* Clear parity enable */
        options.c_iflag &= ~INPCK; /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB); /* Set odd checking*/
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB; /* Enable parity */
        options.c_cflag &= ~PARODD; /* Set event checking*/
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'S':
    case 's': /* as no parity */
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        break;
    }

    /*set stop bits*/
    switch (stopbits){
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        break;
    }

    /* Set input parity option */
    if(parity != 'n')
        options.c_iflag |= INPCK;

    options.c_cc[VTIME] = 3; /* 0.3 seconds */
    options.c_cc[VMIN] = 0; /* at least 0? byte every read */
    options.c_cflag &= ~CRTSCTS; /* disable hardware flow control; */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* raw input */
    options.c_oflag &= ~OPOST; /* raw output */
    options.c_iflag &= ~(ICRNL | IXON);

    tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */
    tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */
    result = tcsetattr(fd, TCSANOW, &options);

    return result;
}

//package for
int open_uart(const char *dev, const int baud, const int data_bits,
        const int stop_bits, const int parity, const int timeout)
{
    int fd = -1;

    if(!dev) {
        //device not exist
        return -2;
    }

    fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
    //fd = open(dev, O_RDWR | O_NOCTTY);

    if(fd <= 0) {
        //device not exist
        return fd;
    }

    fcntl(fd, F_SETFL, 0);

    if(set_uart_speed(fd, baud) != 0) {
        //set speed failed
        close(fd);
        return -3;
    }

    if(set_uart_ctrl_flag(fd, data_bits, stop_bits, parity) != 0) {
        //set control flag failed
        close(fd);
        return -4;
    }

    //set poll
    poll_time_out_ = timeout;

    struct epoll_event event_setup = {
        .events = EPOLLIN,
    };

    fd_epoll_ = epoll_create(1);
    if (fd_epoll_ < 0) {
        perror("epoll_create");
        return -1;
    }

    if (epoll_ctl(fd_epoll_, EPOLL_CTL_ADD, fd, &event_setup)) {
        perror("failed to add uart socket to epoll");
        return 1;
    }

    return fd;
}

void close_uart(const int fd)
{
    /*
    struct epoll_event event_setup = {
        .events = EPOLLIN,
    };
    if (epoll_ctl(fd_epoll_, EPOLL_CTL_DEL, fd, &event_setup)) {
        perror("failed to del uart socket to epoll");
    }
    */

    close(fd_epoll_);
    close(fd);

    fd_epoll_ = -1;
}

int write_uart(const int fd, const uint8_t *data, const size_t len)
{
    int count;

    if(!data || 0 == len || fd < 0) {
        return -1;
    }

    count = write(fd, (char *)data, len);

    if(count != len) {
        return -1;
    }

    return count;
}

int read_uart(const int fd, uint8_t *data, const size_t len)
{
    if(!data || 0 == len || fd < 0) {
        return -1;
    }

    if (fd_epoll_ < 0)
        return -1;

    struct epoll_event events_pending[2];
    int num_events = epoll_wait(fd_epoll_, events_pending, 2, poll_time_out_);
    if (num_events < 0) {
        perror("poll wait uart, fd epoll");
        return -1;
    }

    return read(fd, (char *)data, len);
}

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "modules/chassis/drivers/can/comm/can_lib.h"
#include "modules/chassis/drivers/can/comm/can_ops.h"

#include "modules/chassis/drivers/can/comm/include/linux/can/raw.h"

static int poll_time_out = 2000; //2s
static int can_socket_ = 0;

int can_open()
{
    int s; /* can raw socket */

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("socket");
        return -1;
    }

    can_socket_ = s;

    return s;
}

int set_write()
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    strncpy(ifr.ifr_name, "can0", IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    //ioctl(can_socket_, SIOCGIFINDEX, &ifr);
    if (!ifr.ifr_ifindex) {
        perror("if_nametoindex");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    setsockopt(can_socket_, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    if (bind(can_socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    return 0;
}

int can_config(int s, int baud, int timeout)
{
    if (system("ip link set can0 down") != 0)
        return -1;

    char buf[64] = {0};
    sprintf(buf, "ip link set can0 type can bitrate %d", baud);
    if (system(buf) != 0)
        return -1;

    if (system("ip link set can0 up") != 0)
        return -1;

    if (system("echo 500 > /sys/class/net/can0/tx_queue_len") != 0)
        return -1;

    poll_time_out = timeout;

#if 0
    set_write();
#endif

    return 0;
}

int can_close(int s)
{
    close(s);

    if (system("ip link set can0 down") != 0)
        return -1;

    return 0;
}

int can_send_raw(int id, const unsigned char* buf, const size_t size)
{
    struct canfd_frame frame;

    frame.len = size;
    frame.can_id = id;
#if 0
    set_write();
#endif
    for (size_t x = 0; x < size; x++) {
        frame.data[x] = buf[x];
        //printf("%ld: id %#x, fd %d, val %#x\n", x, id, can_socket_, buf[x]);
    }

    if (write(can_socket_, &frame, CAN_MTU) != CAN_MTU) {
        perror("write");
        return -1;
    }

    return 0;
}

int can_send_string(int s, const char* dev, const char* buf)
{
    int required_mtu;
    struct sockaddr_can addr;
    struct canfd_frame frame;
    struct ifreq ifr;

    required_mtu = parse_canframe(buf, &frame);
    if (!required_mtu){
        fprintf(stderr, "\nWrong CAN-frame format!\n\n");
        return 1;
    }

    strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    if (!ifr.ifr_ifindex) {
        perror("if_nametoindex");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

#if 0
    if (required_mtu > (int)CAN_MTU) {
        if (ioctl(s, SIOCGIFMTU, &ifr) < 0) {
            perror("SIOCGIFMTU");
            return 1;
        }

        if (ifr.ifr_mtu != CANFD_MTU) {
            printf("CAN interface is not CAN FD capable - sorry.\n");
            return 1;
        }

        int enable_canfd = 1;
        /* interface is ok - try to switch the socket into CAN FD mode */
        if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
                    &enable_canfd, sizeof(enable_canfd))){
            printf("error when enabling CAN FD support\n");
            return 1;
        }

        /* ensure discrete CAN FD length values 0..8, 12, 16, 20, 24, 32, 64 */
        frame.len = can_fd_dlc2len(can_fd_len2dlc(frame.len));
    }

    printf("dev: %s, buf: %s, len: %d, id: %x\n",
            dev, buf, frame.len, frame.can_id);
    for (size_t x = 0; x < frame.len; x++) {
        printf("%ld: val %x\n", x, frame.data[x]);
    }
#endif

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (write(s, &frame, required_mtu) != required_mtu) {
        perror("write");
        return 1;
    }

    return 0;
}

size_t can_recv(int s, const char* dev, char* buf)
{
    struct epoll_event event_setup = {
        .events = EPOLLIN,
    };
    struct epoll_event events_pending[16];

    struct sockaddr_can addr;
    char ctrlmsg[CMSG_SPACE(sizeof(struct timeval) +
            3 * sizeof(struct timespec) + sizeof(__u32))];
    struct iovec iov;
    struct msghdr msg;
    struct canfd_frame frame;
    struct ifreq ifr;
    const int canfd_on = 1;
    size_t len = 0;

    int fd_epoll;
    fd_epoll = epoll_create(1);
    if (fd_epoll < 0) {
        perror("epoll_create");
        return 1;
    }

    if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, s, &event_setup)) {
        perror("failed to add socket to epoll");
        return 1;
    }

    addr.can_family = AF_CAN;

    strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    if (strcmp("any", ifr.ifr_name) != 0) {
        if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
            perror("SIOCGIFINDEX");
            //exit(1);
        }
        addr.can_ifindex = ifr.ifr_ifindex;
    } else
        addr.can_ifindex = 0; /* any can interface */

    /* try to switch the socket into CAN FD mode */
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    iov.iov_base = &frame;
    msg.msg_name = &addr;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &ctrlmsg;

    int num_events = epoll_wait(fd_epoll, events_pending, 1024, poll_time_out);
    if (num_events == -1) {
        perror("poll wait");
        return -1;
    }


    for (int i = 0; i < num_events; i++) {
        /* check waiting CAN RAW sockets */
        /* these settings may be modified by recvmsg() */
#if 0
        printf("read, fd: %d, dev: %s, timeout: %d, event: %d\n",
                s, dev, poll_time_out, num_events);
#endif
        iov.iov_len = sizeof(frame);
        msg.msg_namelen = sizeof(addr);
        msg.msg_controllen = sizeof(ctrlmsg);
        msg.msg_flags = 0;

        int nbytes = recvmsg(s, &msg, 0);

        if (nbytes < 0) {
            if ((errno == ENETDOWN)) {
                return -1;
            }
            perror("read");
            return 1;
        }

#if 0
        int maxdlen;
        if ((size_t)nbytes == CAN_MTU)
            maxdlen = CAN_MAX_DLEN;
        else if ((size_t)nbytes == CANFD_MTU)
            maxdlen = CANFD_MAX_DLEN;
        else {
            printf("read: incomplete CAN frame\n");
            return 1;
        }
        sprint_long_canframe(buf, &frame, 0, maxdlen, &len);
#else
        buf[0] = frame.can_id & 0xff;
        buf[1] = (frame.can_id & 0xff00) >> 4;
        len = frame.len + 2;
        for (int i = 0; i < len; i++) {
            buf[2 + i] = frame.data[i];
        }
#endif
    }

    close(fd_epoll);

    return len;
}

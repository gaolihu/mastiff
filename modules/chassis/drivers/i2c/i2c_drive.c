#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <i2c/smbus.h>

static int bus_ = -1;
static int address_ = -1;
static int timeout_ = 2000;

int open_i2c(const char* dev, const int bus,
        const int address, const int timeout) {
    int fd = -1;

    if (!dev) {
        printf("i2c dev null\n");
        return -1;
    }

    if (bus < 0 || bus > 10) {
        printf("i2c bus error!\n");
        return -1;
    }

    if (address < 0 || address > 255) {
        printf("i2c address error!\n");
        return -1;
    }

    address_ = address;

    fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);

    if (fd <= 0) {
        printf("fail to open i2c device: %s!", dev);
        return -1;
    }

    return fd;
}

void close_i2c(const int fd) {
    close(fd);
}

int write_i2c(const int fd, const unsigned char* data, const size_t len) {
    int count = write(fd, (char*)data, len);

    if (count != len) {
        return -1;
    }

    return count;
}

int read_i2c(const int fd, unsigned char* data, const size_t len) {
    if (!data || len == 0 || fd < 0) {
    }

    if (!data || len == 0 || fd < 0) {
    }

    int res = i2c_smbus_read_block_data(fd, bank, cblock);
}

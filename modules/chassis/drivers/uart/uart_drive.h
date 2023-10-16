#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

    int open_uart(const char*, int, int, int, int);
    void close_uart(const int);
    int write_uart(int, const uint8_t*, int);
    int read_uart(int, uint8_t*, int);

#ifdef __cplusplus
}
#endif

#endif

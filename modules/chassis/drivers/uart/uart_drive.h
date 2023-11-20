#ifndef _UART_DRIVE_H
#define _UART_DRIVE_H

#ifdef __cplusplus
extern "C" {
#endif

    int open_uart(const char*, const int, const int, const int, const int, const int);
    void close_uart(const int);
    int write_uart(const int, const uint8_t*, const size_t);
    int read_uart(const int, uint8_t*, const size_t);

#ifdef __cplusplus
}
#endif

#endif //_UART_DRIVE_H

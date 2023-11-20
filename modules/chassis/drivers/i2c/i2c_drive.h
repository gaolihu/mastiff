#ifndef _I2C_DRIVE_H_
#define _I2C_DRIVE_H_

#ifdef __cplusplus
extern "C" {
#endif

int open_i2c(const char*, const int, const int, const int);

void close_i2c(const int);

int write_i2c(const int, const unsigned char*, const size_t);

int read_i2c(const int, unsigned char*, const size_t);

#ifdef __cplusplus
}
#endif

#endif //_I2C_DRIVE_H_

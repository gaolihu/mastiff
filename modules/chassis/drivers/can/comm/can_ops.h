#ifndef __CAN_OP_H_
#define __CAN_OP_H_

#ifdef __cplusplus
extern "C"
{
#endif

    int can_open();
    int can_config(int, int, int);
    int can_close(int);

    /*
     * Description:
     *   Send raw data to by can port
     *   like: 0x0123 0x..
     *
     * @~parm1: can dev id
     * @~parm2: buffer to send data
     * @~parm3: size of buffer to send
     * @~return: 0 OK, -1 NG
     */
    int can_send_raw(int, const unsigned char*, const size_t);

    /*
     * Description:
     *   Send string data to by can port
     *   like: 123#DEADBEEF
     *
     * @~parm1: socket file descriptor
     * @~parm2: can device like 'can0'
     * @~parm3: buffer to send data
     * @~return: 0 OK, -1 NG
     */
    int can_send_string(int, const char*, const char*);

    /*
     * @~parm1: socket file descriptor
     * @~parm2: can device like 'can0'
     * @~parm3: buffer to receive data
     * @~return: size of data get
     */
    size_t can_recv(int, const char*, char*);

#ifdef __cplusplus
}
#endif

#endif //__CAN_OP_H_

#ifndef __DS20270DA_DRIVER_H_
#define __DS20270DA_DRIVER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DS_CODE_MOTOR 0x0
#define DS_CODE_CONTROL 0x1
#define DS_CODE_POS_CIRCLE 0x2
#define DS_CODE_SPEED_CIRCLE 0x3
#define DS_CODE_CURRENT_CIRCLE 0x4
#define DS_CODE_ZERO_RETURN 0x5
#define DS_CODE_INNER_POS 0x6

#define DS_MOTOR_DRIVE_MOTOR_AXLE_1 0x0601
#define DS_MOTOR_DRIVE_MOTOR_AXLE_2 0x0602

#define DS_MOTOR_READ_BOTH_CMD 0xe8
#define DS_MOTOR_WRITE_BOTH_CMD 0xe9
#define DS_MOTOR_WRITE_DIFF_SPEED 0xea
#define DS_MOTOR_WRITE_CANOPEN_1 0x2f
#define DS_MOTOR_WRITE_CANOPEN_2 0x24
#define DS_MOTOR_WRITE_CANOPEN_4 0x23

#define DS_MOTOR_ACK_MOTOR_AXLE_1 0x5081
#define DS_MOTOR_ACK_MOTOR_AXLE_2 0x5082

#define DS_MOTOR_ACK_READ_CMD 0x4b
#define DS_MOTOR_ACK_WRITE_CMD 0x60

struct ServoMotorBriefDes {
    int8_t axle_num;
    int16_t func_addr;
    int16_t reg_value;
} __attribute__((packed));

struct ServoMotorDes {
    int8_t seq;
    int8_t code_m;
    int8_t param_xyz;
    int16_t lower_limit;
    int16_t upper_limit;

    const char* func_str;
    struct ServoMotorBriefDes ax0;
    struct ServoMotorBriefDes ax1;
    struct ServoMotorDes* des_last;
};

#define GEN_SERVO_MOTOR_FN_DES( \
        seq, \
        code_m, \
        param_xyz, \
        last_code_m, \
        last_param_xyz, \
        lower_limit, \
        upper_limit, \
        func_str) \
        struct ServoMotorDes \
        f_##code_m##_##param_xyz = { \
            seq, \
            code_m, \
            param_xyz, \
            lower_limit, \
            upper_limit, \
            func_str, \
            { \
                0, \
                0x2000 + \
                code_m * 0x100 + param_xyz, \
                -11, \
            }, \
            { \
                1, \
                0x3000 + \
                code_m * 0x100 + param_xyz, \
                -11, \
            }, \
            &f_##last_code_m##_##last_param_xyz \
        };

#define GEN_SERVO_MOTOR_DN_DES( \
        seq, \
        param_xyz, \
        last_param_xyz, \
        lower_limit, \
        upper_limit, \
        func_str) \
        struct ServoMotorDes \
        d_##param_xyz = { \
            seq, \
            -11, \
            param_xyz, \
            lower_limit, \
            upper_limit, \
            func_str, \
            { \
                0, \
                0x5000 + \
                param_xyz, \
                -11, \
            }, \
            { \
                1, \
                0x5100 + \
                param_xyz, \
                -11, \
            }, \
            &d_##last_param_xyz \
        };

#define GEN_SERVO_MOTOR_PN_DES( \
        seq, \
        code_m, \
        param_xyz, \
        last_code_m, \
        last_param_xyz, \
        lower_limit, \
        upper_limit, \
        func_str) \
        struct ServoMotorDes \
        p_##code_m##_##param_xyz = { \
            seq, \
            code_m, \
            param_xyz, \
            lower_limit, \
            upper_limit, \
            func_str, \
            { \
                0, \
                0x4000 + \
                code_m * 0x100 + param_xyz, \
                -11, \
            }, \
            { \
                -11, \
                -11, \
                -11, \
            }, \
            &p_##last_code_m##_##last_param_xyz \
        };

#define SHOW_SERVO_MOTOR_FN_DES_HEX(code_m, param_xyz) \
    do { \
        struct ServoMotorDes* x = \
        &f_##code_m##_##param_xyz; \
        printf("f0/1.%d.%#x " \
                "address: %#x/%#x, value: %d/%d(limit: %d ~ %d), des: %s\n", \
                code_m, param_xyz, \
                x->ax0.func_addr, \
                x->ax1.func_addr, \
                x->ax0.reg_value, \
                x->ax1.reg_value, \
                x->lower_limit, \
                x->upper_limit, \
                x->func_str); \
    } while (0)

//interfaces
//initilize
void init_servo_motor_fn_des();
void init_servo_motor_dn_des();
void init_servo_motor_pn_des();

//show information
void show_all_servo_motor_fn_des();
void show_all_servo_motor_dn_des();
void show_all_servo_motor_pn_des();

//get addresses
void get_servo_motor_all_fn_addrs(int16_t* addr, int* size);
void get_servo_motor_all_dn_addrs(int16_t* addr, int* size);
void get_servo_motor_all_pn_addrs(int16_t* addr, int* size);

//set servo motor parameters
void config_servo_motor_parameters(const int32_t, const int32_t, const int32_t, const int32_t, const int32_t, const int32_t);

//parse protocols
void parse_servo_motor_info(const uint8_t* addr, size_t len,
        int*, int*, int*, int*, int*, int*);

//control APIs
void config_servo_motor_init(int16_t* addr, int* value, int* size);
void config_servo_motor_disable(int16_t* addr, int* value, int* size);
void config_servo_motor_stop(int16_t* addr, int* value, int* size);
void config_servo_motor_wheel_speed(int16_t* addr, int* value, int* size);
void config_servo_motor_wheel_reverse(int16_t* addr, int* value, int* size);

//canopen config
void config_servo_motor_canopen_enable(int16_t* addr, int* value, int* size);
void config_servo_motor_canopen_mode(uint8_t* cmd, int16_t* addr, uint8_t* sub_id, int* value, int* size);
void config_servo_motor_acc_moderate_time(uint8_t* cmd, int16_t* addr, uint8_t* sub_id, int* value, int* size);
void config_servo_motor_tpdo_rpdo(uint8_t* cmd, int16_t* addr, uint8_t* sub_id, int* value, int* size);

#ifdef __cplusplus
}
#endif

#endif //__DS20270DA_DRIVER_H_

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"],)

## Add a micro for example
## CHSS_COPTS = ['-DUART_DEV=\\"/dev/ttyS0\\"']
##

CHSS_COPTS = '-DCHSS_PKG_DBG'

### test ###
cc_binary(
    name = "gs2_serial_read",
    srcs = [
        "test/gs2_serial_read.cc",
    ],
    copts = [
        '-DCHSS_PKG',
        '-DPKG_NAME=\\"chss_pkg\\"',
        CHSS_COPTS,
    ],
    deps = [
        ":chassis",
        ":serial_data",
        ":uart_drive",
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
    ]
)

#### main target ####
cc_library(
    name = "chassis",
    deps = [
        ":chss_pkg",
        #":libchss_component.so",
    ],
)

# executive #
cc_binary(
    name = "chss_pkg",
    srcs = [
        "chss_cmpnt.cc"
    ],
    copts = [
        '-DCHSS_PKG',
        '-DPKG_NAME=\\"chss_pkg\\"',
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":dispatch_chs",
        ":receive_ctrl",
    ]
)

# library #
cc_binary(
    name = "libChsCpnt.so",
    linkshared = True,
    linkstatic = False,
    deps = [
        ":chss_pkg_lib_so",
    ]
)

cc_library(
    name = "chss_pkg_lib_so",
    srcs = [
        "chss_cmpnt.cc"
    ],
    copts = [
        '-DCHSS_PKG',
        '-DPKG_NAME=\\"chss_pkg\\"',
        CHSS_COPTS,
    ],
)

cc_library(
    name = "dispatch_chs",
    srcs = [
    ],
    hdrs = [
        "dispatch_chs.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
        ":msg_transfer",
    ]
)

cc_library(
    name = "receive_ctrl",
    srcs = [
    ],
    hdrs = [
        "receive_ctrl.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
        ":msg_transfer",
        ":device_manager",
    ]
)

######## ######## 1 devices ######## ########
cc_library(
    name = "msg_transfer",
    srcs = [
    ],
    hdrs = [
        "devices/msg_transfer.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:input_output_chs_cc_proto",
        ":data_transact",
        ":key_simulate",
    ]
)

cc_library(
    name = "device_manager",
    srcs = [
        "devices/device_manager.cc",
    ],
    hdrs = [
        "devices/device_manager.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:input_output_chs_cc_proto",

        ":dev_servo",
        ":audio_play",
        ":dev_wireless",
        ":dev_lidar",
        ":dev_mcu",
        ":dev_camera",
        ":dev_gpio",
        ":line_laser",
        ":dev_imu",
    ]
)

cc_library(
    name = "data_transact",
    srcs = [
        "devices/data_transact.cc",
    ],
    hdrs = [
        "devices/data_transact.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:input_output_chs_cc_proto",
        ":device_base_itf",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "device_base_itf",
    srcs = [
    ],
    hdrs = [
        "devices/device_base_itf.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
    ]
)

cc_library(
    name = "key_simulate",
    srcs = [
        "devices/key_simulate.cc",
    ],
    hdrs = [
        "devices/key_simulate.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:input_output_chs_cc_proto",
        ":dev_tty",
    ]
)

######## dev libs ########
cc_library(
    name = "dev_tty",
    srcs = [
        "devices/dev_libs/dev_tty.cc",
    ],
    hdrs = [
        "devices/dev_libs/dev_tty.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
    ]
)

cc_library(
    name = "dev_servo",
    srcs = [
        "devices/dev_libs/dev_servo.cc",
    ],
    hdrs = [
        "devices/dev_libs/dev_servo.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":servo_parser",
    ]
)
cc_library(
    name = "audio_play",
    srcs = [
        "devices/dev_libs/audio_play.cc",
    ],
    hdrs = [
        "devices/dev_libs/audio_play.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":audio_parser",
    ]
)

cc_library(
    name = "dev_wireless",
    srcs = [
        "devices/dev_libs/dev_wireless.cc",
    ],
    hdrs = [
        "devices/dev_libs/dev_wireless.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":wireless_parser",
    ]
)

cc_library(
    name = "dev_mcu",
    srcs = [
        "devices/dev_libs/dev_mcu.cc",
    ],
    hdrs = [
        "devices/dev_libs/dev_mcu.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":mcu_parser",
    ]
)

cc_library(
    name = "dev_camera",
    srcs = [
        "devices/dev_libs/dev_camera.cc",
    ],
    hdrs = [
        "devices/dev_libs/dev_camera.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":camera_parser",
    ]
)

cc_library(
    name = "dev_gpio",
    srcs = [
        "devices/dev_libs/dev_gpio.cc",
    ],
    hdrs = [
        "devices/dev_libs/dev_gpio.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":gpio_parser",
    ]
)

cc_library(
    name = "line_laser",
    srcs = [
        "devices/dev_libs/line_laser.cc",
    ],
    hdrs = [
        "devices/dev_libs/line_laser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":linelaser_parser",
    ]
)

cc_library(
    name = "dev_imu",
    srcs = [
        "devices/dev_libs/dev_imu.cc",
    ],
    hdrs = [
        "devices/dev_libs/dev_imu.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":imu_parser",
    ]
)

cc_library(
    name = "dev_lidar",
    srcs = [
        "devices/dev_libs/dev_lidar.cc",
    ],
    hdrs = [
        "devices/dev_libs/dev_lidar.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":device_base_itf",
        ":data_transact",

        ":lidar_parser",
    ]
)
######## dev libs ########
######## ######## 1 devices ######## ########

######## ######## 2 parser ######## ########
cc_library(
    name = "servo_parser",
    srcs = [
        "parser/parse_libs/servo_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/servo_parser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "@org_tuxfamily_eigen//:eigen",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":parser_base_itf",
        ":ds20270da_driver",
    ]
)

cc_library(
    name = "ds20270da_driver",
    srcs = [
        "parser/parse_libs/ds20270da_driver.c",
    ],
    hdrs = [
        "parser/parse_libs/ds20270da_driver.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
    ]
)

cc_library(
    name = "audio_parser",
    srcs = [
        "parser/parse_libs/audio_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/audio_parser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "wireless_parser",
    srcs = [
        "parser/parse_libs/wireless_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/wireless_parser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "lidar_parser",
    srcs = [
        "parser/parse_libs/lidar_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/lidar_parser.h",
        "parser/parse_libs/lidar_50cr.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        "//modules/chassis/proto:frame_up_stream_cc_proto",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "mcu_parser",
    srcs = [
        "parser/parse_libs/mcu_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/mcu_parser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "camera_parser",
    srcs = [
        "parser/parse_libs/camera_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/camera_parser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "gpio_parser",
    srcs = [
        "parser/parse_libs/gpio_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/gpio_parser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "linelaser_parser",
    srcs = [
        "parser/parse_libs/linelaser_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/linelaser_parser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "imu_parser",
    srcs = [
        "parser/parse_libs/imu_parser.cc",
    ],
    hdrs = [
        "parser/parse_libs/imu_parser.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":parser_base_itf",
    ]
)

cc_library(
    name = "parser_base_itf",
    srcs = [
    ],
    hdrs = [
        "parser/parser_base_itf.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        "//modules/chassis/proto:frame_down_stream_cc_proto",
        ":raw_manage",
        ":raw_packer",
        ":servo_packer",
        ":llaser_packer",
        ":imu_packer",
        ":circular_buffer",
    ]
)

cc_library(
    name = "circular_buffer",
    srcs = [
    ],
    hdrs = [
        "parser/circular_buffer.h",
    ],
    copts = [
        '-DCBUF_DBG',
    ],
    deps = [
        "//cyber",
    ]
)

cc_library(
    name = "raw_packer",
    srcs = [
        "parser/pack_libs/raw_packer.cc",
    ],
    hdrs = [
        "parser/pack_libs/raw_packer.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        ":packer_base_itf",
    ]
)

cc_library(
    name = "servo_packer",
    srcs = [
        "parser/pack_libs/servo_packer.cc",
    ],
    hdrs = [
        "parser/pack_libs/servo_packer.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        ":packer_base_itf",
        ":ds20270da_driver",
    ]
)

cc_library(
    name = "llaser_packer",
    srcs = [
        "parser/pack_libs/llaser_packer.cc",
    ],
    hdrs = [
        "parser/pack_libs/llaser_packer.h",
        "parser/parse_libs/ydlidar_protocol.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        ":packer_base_itf",
    ]
)

cc_library(
    name = "imu_packer",
    srcs = [
        "parser/pack_libs/imu_packer.cc",
    ],
    hdrs = [
        "parser/pack_libs/imu_packer.h",
        "parser/parse_libs/ydlidar_protocol.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        ":packer_base_itf",
    ]
)

cc_library(
    name = "packer_base_itf",
    srcs = [
    ],
    hdrs = [
        "parser/packer_base_itf.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:frame_down_stream_cc_proto",
    ]
)

cc_library(
    name = "raw_manage",
    srcs = [
        "parser/raw_manage.cc"
    ],
    hdrs = [
        "parser/raw_manage.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":can_data",
        ":soc_data",
        ":serial_data",
    ]
)
######## ######## 2 parser ######## ########

######## ######## 3 drivers ######## ########
######## can ########
cc_library(
    name = "driver_data_itf",
    srcs = [
    ],
    hdrs = [
        "drivers/driver_data_itf.h",
    ],
    copts = [
        '-DUSE_CYBER_TIMER',
    ],
    deps = [
        "//cyber",
    ]
)

cc_library(
    name = "can_data",
    srcs = [
        "drivers/can/can_data.cc"
    ],
    hdrs = [
        "drivers/can/can_data.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":driver_data_itf",
        ":can_ops",
    ]
)

cc_library(
    name = "can_ops",
    srcs = [
        "drivers/can/comm/can_ops.c"
    ],
    hdrs = [
        "drivers/can/comm/can_ops.h",
        "drivers/can/comm/include/linux/can/raw.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        ":can_lib",
    ]
)

cc_library(
    name = "can_lib",
    srcs = [
        "drivers/can/comm/can_lib.c"
    ],
    hdrs = [
        "drivers/can/comm/can_lib.h",
        "drivers/can/comm/include/linux/can.h",
        "drivers/can/comm/include/linux/can/error.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
    ]
)
######## can ########

######## soc ########
cc_library(
    name = "soc_data",
    srcs = [
        "drivers/soc/soc_data.cc"
    ],
    hdrs = [
        "drivers/soc/soc_data.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
        ":driver_data_itf",
        ":network",
        ":camera",
    ]
)

cc_library(
    name = "network",
    srcs = glob([
        "drivers/soc/network/*.cc",
    ]),
    hdrs = glob([
        "drivers/soc/network/*.h",
    ]),
    deps = [
        "//cyber",
        #"//modules/chassis/proto:chassis_config_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
    ]
)

cc_library(
    name = "camera",
    deps = [
        ":angstrong_camera_node",
    ]
)

cc_library(
    name = "angstrong_camera_node",
    srcs = glob([
        "drivers/soc/camera/angstrong_camera_node/*.cc",
    ]),
    hdrs = glob([
        "drivers/soc/soc_defs.h",
        "drivers/soc/camera/angstrong_camera_node/*.h",
    ]),
    deps = [
        "//cyber",
        "@opencv4_8//:opencv4",
        "@angstrong_camera//:angstrong",
        "//modules/chassis/proto:chassis_config_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
    ]
)
######## soc ########

######## serial ########
cc_library(
    name = "serial_data",
    srcs = [
        "drivers/uart/serial_data.cc"
    ],
    hdrs = [
        "drivers/uart/serial_data.h",
    ],
    copts = [
        CHSS_COPTS,
        #'-DSERIAL_PORTING',
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:chassis_config_cc_proto",
        ":driver_data_itf",
        ":uart_drive",
    ]
)

cc_library(
    name = "uart_drive",
    srcs = [
        "drivers/uart/uart_drive.c",

        "drivers/uart/serial.cpp",
        "drivers/uart/unix_serial.cpp"
    ],
    hdrs = [
        "drivers/uart/uart_drive.h",

        "drivers/uart/ChannelDevice.h",
        "drivers/uart/serial.h",
        "drivers/uart/unix_serial.h",
    ],
    copts = [
        CHSS_COPTS,
    ],
    deps = [
        "//cyber",
    ]
)
######## serial ########
######## ######## 3 drivers ######## ########

cpplint()

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"],)

#CAMBRIAN_COPTS = ''
CAMBRIAN_COPTS = '-DCAMB_PKG_DBG'

#### main target library ####
cc_library(
    name = "cambrian",
    deps = [
        "camb_pkg",
        #"libCamCpnt.so",
    ],
)

#### main target executive & dynamic library ####
cc_binary(
    name = "camb_pkg",
    srcs = [
        "cambrian.cc"
    ],
    copts = [
        "-DCAMBRIAN_PKG",
        '-DPKG_NAME=\\"camb_pkg\\"',
    ],
    deps = [
        "//cyber",
        "//modules/cambrian/proto:cambrian_arch_cc_proto",
        ":dispatcher",
        ":receiver",
    ]
)

cc_binary(
    name = "libCamCpnt.so",
    linkshared = True,
    linkstatic = False,
    deps = [
        ":camb_pkg_lib_so",
    ],
)

cc_library(
    name = "camb_pkg_lib_so",
    srcs = [
        "cambrian.cc"
    ],
    deps = [
        "//cyber",
        "//cyber/mainboard",
    ],
)

#### message entry ####
cc_library(
    name = "receiver",
    srcs = [
    ],
    hdrs = [
        "receiver.h",
    ],
    copts = [
        #CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/chassis/proto:external_msg_cc_proto",
        ":msgs_recv_tmpl",
    ],
)

cc_library(
    name = "dispatcher",
    srcs = [
    ],
    hdrs = [
        "dispatcher.h",
    ],
    copts = [
        #CAMBRIAN_COPTS,
    ],
    deps = [
        "//modules/chassis/proto:external_msg_cc_proto",
        ":msgs_send_tmpl",
    ],
)

#### brain ####
cc_library(
    name = "msgs_impl_itf",
    srcs = [
    ],
    hdrs = [
        "brain/msgs_impl_itf.h",
    ],
    copts = [
    ],
    deps = [
        "//cyber",
    ],
)

cc_library(
    name = "msgs_send_tmpl",
    srcs = [
    ],
    hdrs = [
        "brain/msgs_send_tmpl.h",
    ],
    copts = [
    ],
    deps = [
        "//cyber",
        ":msgs_impl_itf",
        ":transactor",
    ],
)

cc_library(
    name = "msgs_recv_tmpl",
    srcs = [
    ],
    hdrs = [
        "brain/msgs_recv_tmpl.h",
    ],
    copts = [
    ],
    deps = [
        "//cyber",
        ":msgs_impl_itf",
        ":transactor",
    ],
)

cc_library(
    name = "transactor",
    srcs = [
    ],
    hdrs = [
        "brain/transactor.h",
    ],
    copts = [
        #CAMBRIAN_COPTS,
    ],
    deps = [
        ":abstract_bot",
    ],
)

### robot ###
cc_library(
    name = "bot_itf_impl",
    srcs = [
    ],
    hdrs = [
        "brain/bot_itf_impl.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/cambrian/proto:cambrian_arch_cc_proto",
        ":sys_processor",
    ],
)

cc_library(
    name = "abstract_bot",
    srcs = [
        "brain/abstract_bot.cc",
    ],
    hdrs = [
        "brain/abstract_bot.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        ":bot_itf_impl",
    ],
)

cc_library(
    name = "sys_processor",
    srcs = [
        "brain/sys_processor.cc",
    ],
    hdrs = [
        "brain/sys_processor.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//modules/cambrian/proto:cambrian_arch_cc_proto",
        ":implement_hub",
        ":visual_client",
        ":dbase_client",
    ],
)

cc_library(
    name = "implement_hub",
    srcs = [
        "brain/implement_hub.cc",
    ],
    hdrs = [
        "brain/chassis_itf.h",
        "brain/implement_hub.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//modules/cambrian/proto:cambrian_arch_cc_proto",
        "//modules/cambrian/proto:cambrian_data_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
        ":infer_machine",
        ":bizlogic_itf",
    ],
)

cc_library(
    name = "infer_machine",
    srcs = [
        "brain/infer_machine.cc",
    ],
    hdrs = [
        "brain/infer_machine.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/cambrian/proto:cambrian_data_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
        ":message_pool",
    ],
)

cc_library(
    name = "message_pool",
    srcs = [
        "brain/message_pool.cc",
    ],
    hdrs = [
        "brain/message_pool.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        ":data_feedback",
        ":signals_kick",
    ],
)

cc_library(
    name = "data_feedback",
    srcs = [
        "brain/data_feedback.cc",
    ],
    hdrs = [
        "brain/data_feedback.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/cambrian/proto:cambrian_data_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
    ],
)

cc_library(
    name = "signals_kick",
    srcs = [
        "brain/signals_kick.cc",
    ],
    hdrs = [
        "brain/signals_kick.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/cambrian/proto:cambrian_data_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
    ],
)

#### biz ####
cc_library(
    name = "bizlogic_itf",
    srcs = [
    ],
    hdrs = [
        "brain/bizlogic_itf.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        ":bizness_client",
    ],
)

cc_library(
    name = "bizness_client",
    srcs = [
        "brain/bizlogic/bizness_client.cc",
    ],
    hdrs = [
        "brain/bizlogic/bizness_client.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
    ],
)
#### biz ####
#### brain ####

#### visualization ####
cc_library(
    name = "visual_client",
    srcs = [
        "visualize/visual_client.cc",
    ],
    hdrs = [
        "visualize/visual_client.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        ":web_server",
    ],
)

cc_library(
    name = "web_server",
    srcs = [
        "visualize/web_server.cc",
    ],
    hdrs = [
        "visualize/web_server.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        ":wbskt_server",
        #":grpc_server",
        #":http_server",
        #":mqqt_server",
    ],
)

cc_library(
    name = "grpc_server",
    srcs = [
        "visualize/websocket/grpc_server.cc",
    ],
    hdrs = [
        "visualize/websocket/grpc_server.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        ":webase_itf",
    ],
)

cc_library(
    name = "mqqt_server",
    srcs = [
        "visualize/websocket/mqqt_server.cc",
    ],
    hdrs = [
        "visualize/websocket/mqqt_server.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        ":webase_itf",
    ],
)

cc_library(
    name = "http_server",
    srcs = [
        "visualize/websocket/http_server.cc",
    ],
    hdrs = [
        "visualize/websocket/http_server.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        ":webase_itf",
    ],
)

cc_library(
    name = "wbskt_server",
    srcs = [
        "visualize/websocket/wbskt_server.cc",
    ],
    hdrs = [
        "visualize/websocket/wbskt_server.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "@boost//:asio",
        "@websocket_cpp//:websocket",
        ":webase_itf",
    ],
)

cc_library(
    name = "webase_itf",
    srcs = [
    ],
    hdrs = [
        "visualize/webase_itf.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        ":remote_parse",
    ],
)

cc_library(
    name = "remote_parse",
    srcs = [
        "visualize/remote_parse.cc",
    ],
    hdrs = [
        "visualize/remote_parse.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/cambrian/proto:cambrian_data_cc_proto",
        "//modules/cambrian/proto:cambrian_arch_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
    ],
)
#### visualization ####

#### database ####
cc_library(
    name = "dbase_client",
    srcs = [
        "storage/dbase_client.cc",
    ],
    hdrs = [
        "storage/dbase_client.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
        '-DBOT_NAME=\\"Mastiff\\ Bot\\"',
        '-DBOT_TYPE=\\"ROBOT\\ EXP\\"',
        '-DBOT_SN=\\"Mstf-SN\\"',
        '-DBOT_UUID=\\"Mastiff-UUID\\"',
        '-DPRODUCT_KEY=\\"Mstf-KEY\\"',
        '-DSW_VERSION=\\"Mastiff\\ 0.7.5\\"',
        '-DSYS_VERSION=\\"Linux\\ 5.10\\"',
        '-DMCU_VERSION=\\"2.3.0\\"',
        '-DRLS_DATE=\\"2024/1/3\\"',
        '-DVOICE_PATH=\\"/data/audio/voice/\\"',
        '-DOTA_PATH=\\"/tmp/\\"',
        '-DSELF_AP=\\"Mstf-\\"',
    ],
    deps = [
        ":db_operation",
    ],
)

cc_library(
    name = "db_operation",
    srcs = [
        "storage/db_operation.cc",
    ],
    hdrs = [
        "storage/db_operation.h",
    ],
    copts = [
        CAMBRIAN_COPTS,
    ],
    deps = [
        "//cyber",
        "//modules/cambrian/proto:cambrian_data_cc_proto",
        "//modules/cambrian/proto:cambrian_arch_cc_proto",
        #"//modules/chassis/proto:input_output_chs_cc_proto",
        "@sqlite3//:sq3",
    ],
)
#### database ####

cpplint()

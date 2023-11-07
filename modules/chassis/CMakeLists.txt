add_compile_definitions(CHSS_PKG_DBG)

add_subdirectory(proto)

find_package(absl REQUIRED)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE devices_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/devices/*.c
)
file(GLOB_RECURSE drivers_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/*.c
)
file(GLOB_RECURSE parser_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.c
)
file(GLOB_RECURSE proto_srcs
    ${PROTOC_GEN_PATH}/modules/*.cc
)

include_directories(
    ${RootPath}
    ${THIRD_PARTY_SYSROOT}/include
    ${OpenCV_INCLUDE_DIRS}
    ${THIRD_PARTY_SYSROOT}/include/eigen3
    ${THIRD_PARTY_SYSROOT}/include/angstrong_sdk
    ${PROTOC_GEN_PATH}
)
add_library(chassis STATIC
    ${devices_srcs} ${drivers_srcs} ${parser_srcs}
    ${proto_srcs}
)
target_link_libraries(chassis PRIVATE
    -L${CMAKE_INSTALL_PREFIX}/lib
    -L${THIRD_PARTY_SYSROOT}/lib
    -L${THIRD_PARTY_SYSROOT}/lib/angstrong_sdk
    # cyber
    cyber cyber_common
    uuid fastrtps fastcdr glog gflags gtest protobuf
    absl::strings
    atomic
    # angstrong sdk
    AngstrongCameraSdk Filt asusb turbojpeg asuvc
    dtofang_alg AngKondyorArith zmq
    ${OpenCV_LIBS}
)
# 复制库到指定目录
add_custom_command(TARGET chassis POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:chassis> ${CMAKE_INSTALL_PREFIX}/lib/$<TARGET_FILE_NAME:chassis>
)

add_executable(chss_cmpnt chss_cmpnt.cc
    dispatch_chs.h receive_ctrl.h
)
target_link_libraries(chss_cmpnt
    chassis
    cyber cyber_common
    uuid fastrtps fastcdr glog gflags gtest protobuf
    osqp qdldl sqlite3 ceres
    boost_system boost_filesystem
    absl::strings absl::numeric
)

add_subdirectory(test)

install(
    TARGETS chassis chss_cmpnt
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

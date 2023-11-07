# add_compile_definitions(CHSS_PKG_DBG)

find_package(OpenCV REQUIRED)

add_executable(gs2_serial_read gs2_serial_read.cc)
target_link_libraries(gs2_serial_read
    -L${CMAKE_INSTALL_PREFIX}/lib
    -L${THIRD_PARTY_SYSROOT}/lib
    chassis
    cyber cyber_common
    uuid fastrtps fastcdr glog gflags gtest protobuf
    osqp qdldl sqlite3 ceres
    boost_system boost_filesystem
    absl::strings absl::numeric
)
add_executable(wifi_tool_test wifi_tool_test.cc)
target_link_libraries(wifi_tool_test
    -L${CMAKE_INSTALL_PREFIX}/lib
    -L${THIRD_PARTY_SYSROOT}/lib
    chassis
    cyber cyber_common
    uuid fastrtps fastcdr glog gflags gtest protobuf
    osqp qdldl sqlite3 ceres
    boost_system boost_filesystem
    absl::strings absl::numeric
)

# camera node test
include_directories(
    ${THIRD_PARTY_SYSROOT}/include/angstrong_sdk
    ${OpenCV_INCLUDES}
)
add_executable(angstrong_camera_node angstrong_camera_node.cc)
target_link_libraries(angstrong_camera_node
    -L${CMAKE_INSTALL_PREFIX}/lib
    -L${THIRD_PARTY_SYSROOT}/lib
    -L${THIRD_PARTY_SYSROOT}/lib/angstrong_sdk
    chassis
    cyber cyber_common
    uuid fastrtps fastcdr glog gflags gtest protobuf
    osqp qdldl sqlite3 ceres
    boost_system boost_filesystem
    absl::strings absl::numeric

    AngstrongCameraSdk Filt asusb turbojpeg asuvc
    dtofang_alg AngKondyorArith zmq
    ${OpenCV_LIBS}
)
install(
    TARGETS gs2_serial_read wifi_tool_test
    angstrong_camera_node
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

file(GLOB_RECURSE proto_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/*.proto
)

execute_process(
    COMMAND ${PROTOC_BIN} --cpp_out=${RootPath}/proto --proto_path=${RootPath} -I ${THIRD_PARTY_SYSROOT}/include ${proto_srcs}
    WORKING_DIRECTORY ${RootPath}
    RESULT_VARIABLE RET
)

if(NOT RET EQUAL "0")
    message(FATAL_ERROR "protoc executed fail")
endif()

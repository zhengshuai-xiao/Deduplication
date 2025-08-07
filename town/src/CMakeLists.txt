
cmake_minimum_required(VERSION 3.5.1)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set (CMAKE_CXX_STANDARD 17)
endif()

find_package(Threads REQUIRED)
if(TRUE)
  set(protobuf_MODULE_COMPATIBLE TRUE)
  find_package(Protobuf CONFIG REQUIRED)
  message(STATUS "Using protobuf ${Protobuf_VERSION}")

  set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
  set(_REFLECTION gRPC::grpc++_reflection)
  if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
  else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
  endif()

  # Find gRPC installation
  # Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
  find_package(gRPC CONFIG REQUIRED)
  message(STATUS "Using gRPC ${gRPC_VERSION}")

  set(_GRPC_GRPCPP gRPC::grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
  endif()
endif()

include_directories(${PROJECT_SOURCE_DIR})

# Proto file
get_filename_component(hw_proto "protos/storageserver.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/storageserver.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/storageserver.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/storageserver.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/storageserver.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
#include_directories(BEFORE  ${PROJECT_SOURCE_DIR}/common)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "xzs Include directories: ${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(hw_grpc_proto 
  ${hw_grpc_srcs}
  ${hw_grpc_hdrs}
  ${hw_proto_srcs}
  ${hw_proto_hdrs})
target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_custom_target(generate_files ALL
    DEPENDS ${hw_proto_hdrs} ${hw_grpc_hdrs}
)

add_custom_command(
    OUTPUT ${DEST_DIR}/output.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${hw_grpc_hdrs} ${PROJECT_SOURCE_DIR}/include/
    COMMAND ${CMAKE_COMMAND} -E copy ${hw_proto_hdrs} ${PROJECT_SOURCE_DIR}/include/
    DEPENDS generate_files  # 关键：等待生成完成后再复制
    COMMENT "Copying generated file to destination"
)

add_library(myrpc 
  "rpc/rpc_server.cc"
  "rpc/rpc_server.h"
  "rpc/rpc_client.cc"
)
target_link_libraries(myrpc
  hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  common)

# Targets storage_[async_](client|server)
add_executable(downtown "downtown.cc")
target_link_libraries(downtown
    myrpc
    common)

add_executable(test_rpc_client "rpc/test_rpc_client.cc")
target_link_libraries(test_rpc_client
    myrpc
    common)
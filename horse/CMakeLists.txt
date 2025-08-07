
cmake_minimum_required(VERSION 3.5.1)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set (CMAKE_CXX_STANDARD 17)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

find_package(Threads REQUIRED)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${PROJECT_SOURCE_DIR})

add_executable(horse "horse.cc")
target_link_libraries(horse
    hw_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    myrpc
    common)
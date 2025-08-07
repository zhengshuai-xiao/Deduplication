
cmake_minimum_required(VERSION 3.5.1)

project(Deduplication C CXX)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set (CMAKE_CXX_STANDARD 17)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(BUILD_SHARED_LIBS ON)

include_directories(${PROJECT_SOURCE_DIR})
message(STATUS "xzs1: ${PROJECT_SOURCE_DIR}")

find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_subdirectory(rpc/grpc)
add_subdirectory(common)
add_subdirectory(town/src/)
add_subdirectory(horse)

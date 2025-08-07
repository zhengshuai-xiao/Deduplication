cmake_minimum_required(VERSION 3.5.1)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set (CMAKE_CXX_STANDARD 17)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

find_package(Threads REQUIRED)


add_library(common 
logger.cc CDC.cc fixedCDC.cc segBuffer.cc)
target_link_libraries(common pthread)
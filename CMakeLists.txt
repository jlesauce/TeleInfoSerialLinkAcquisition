cmake_minimum_required(VERSION 3.16)
project(TeleInfoMonitor)

set(CMAKE_CXX_STANDARD 20)

set(LIB_PATH /home/jlesauce-dev/workspace-raspberry/rootfs/usr/lib)

# Search WiringPi library
find_library(WIRINGPI_LIB wiringPi HINTS ${LIB_PATH} NO_CMAKE_FIND_ROOT_PATH)
if(NOT WIRINGPI_LIB)
    message(FATAL_ERROR "wiringPi library not found")
endif()

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} ${WIRINGPI_LIB})

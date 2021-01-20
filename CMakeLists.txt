cmake_minimum_required(VERSION 3.16)
project(TeleInfoMonitor)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(LIB_PATH ${HOME}/workspace-raspberry/rootfs/usr/lib)

# Search WiringPi library
find_library(WIRINGPI_LIB wiringPi HINTS ${LIB_PATH} NO_CMAKE_FIND_ROOT_PATH)
if(NOT WIRINGPI_LIB)
    message(FATAL_ERROR "wiringPi library not found")
endif()

include_directories(src)

add_subdirectory(src)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${WIRINGPI_LIB})

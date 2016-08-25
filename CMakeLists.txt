cmake_minimum_required(VERSION 2.8.7)
project(reginject C CXX)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -stdlib=libc++ ")

include_directories("${CMAKE_SOURCE_DIR}/src")

#file(GLOB liberation_SRC
#    "*.h"
#    "*.cpp"
#)

add_executable(reginject src/main.cc src/Process.cc src/ExceptionHandler.cc
                src/mach_excServer.c src/mach_excUser.c
                src/BreakpointHandler.cc src/breakpoint.cc src/AArch64/AArch64Host.cc
                src/ARMv7/ARMv7Host.cc src/ARMv7/ARMv7Breakpoint.cc src/AArch64/AArch64Breakpoint.cc)

add_custom_target(install
    COMMAND make
    COMMAND ldid -S ${CMAKE_SOURCE_DIR}/build/reginject
    COMMAND scp ${CMAKE_SOURCE_DIR}/build/reginject root@$ENV{THEOS_DEVICE_IP}:/usr/bin/reginject
)

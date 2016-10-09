cmake_minimum_required(VERSION 2.8.7)
project(reginject C CXX)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -stdlib=libc++ ")

include_directories("${CMAKE_SOURCE_DIR}/src")

file(GLOB liberation_SRC
   "src/*.cc"
   "src/*.c"
)

if (NOT DEFINED IOS_PLATFORM)
    file(GLOB liberation_arch_specific_SRC
        "src/x86_64/*.cc"
    )
else()
    file(GLOB liberation_arch_specific_SRC
        "src/ARMv7/*.cc"
        "src/AArch64/*.cc"
    )
endif()

file(GLOB liberation_test_SRC
        "tests/*.cc"
)

add_library(reginject STATIC ${liberation_SRC} ${liberation_arch_specific_SRC})

add_executable(reginject_test ${liberation_test_SRC})

target_link_libraries(reginject_test reginject)

add_custom_target(install
    COMMAND make
    COMMAND ldid -S ${CMAKE_SOURCE_DIR}/build/reginject
    COMMAND scp ${CMAKE_SOURCE_DIR}/build/reginject root@$ENV{THEOS_DEVICE_IP}:/usr/bin/reginject
)

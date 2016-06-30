
cmake_minimum_required(VERSION 2.8.8)
project(Liberation CXX)

set(LIBERATION_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3 -std=c++11 -stdlib=libc++ ")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBERATION_OUTPUT_DIRECTORY})


if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/keystone/include")
include_directories("$ENV{THEOS}/vendor/include")

file(GLOB liberation_SRC
    "source/*.h"
    "source/*.cpp"
    "source/*.mm"
)

add_subdirectory(keystone)
set_target_properties(keystone PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${LIBERATION_OUTPUT_DIRECTORY}")
add_library(Liberation STATIC ${liberation_SRC})
target_link_libraries(Liberation keystone)

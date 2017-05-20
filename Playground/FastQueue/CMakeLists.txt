cmake_minimum_required(VERSION 2.8)
project(FastQueues)


set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")

set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
include_directories(SYSTEM /usr/local/include /usr/include)

set(FastQueues_INCLUDES
    Classic.h)


set(FastQueues_SOURCES
    Classic.cpp
    main.cpp)

add_executable(FastQueues ${FastQueues_SOURCES})
set_target_properties(FastQueues PROPERTIES COMPILE_FLAGS "-std=c++14 -O2 -g -march=native -Wall -Wextra -Werror -Wno-missing-braces ")
target_link_libraries(FastQueues "-L/usr/local/lib -lpcap")

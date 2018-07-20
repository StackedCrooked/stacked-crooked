cmake_minimum_required(VERSION 3.0)

project(NetworkEmulation)

set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
include_directories(SYSTEM /usr/local/include /usr/include)

set(NetworkEmulation_INCLUDES
    Multiplexer.h
    Switch.h)

set(NetworkEmulation_SOURCES
    Multiplexer.cpp
    Switch.cpp
    main.cpp)

add_executable(NetworkEmulation ${NetworkEmulation_SOURCES})
set_target_properties(NetworkEmulation PROPERTIES COMPILE_FLAGS "-std=c++11 -O0 -g -march=native -Wall -Wextra -Werror ")
target_link_libraries(NetworkEmulation "-L/usr/local/lib")

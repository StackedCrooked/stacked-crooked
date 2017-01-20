cmake_minimum_required(VERSION 2.8)
project(FilteringApp)


set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")

set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
include_directories(SYSTEM /usr/local/include /usr/include)

set(FilteringApp_INCLUDES
    Range.h)


set(FilteringApp_SOURCES
    BBInterface.cpp
    BBPort.cpp
    BBServer.cpp
    PhysicalInterface.cpp
    main.cpp)

add_executable(FilteringApp ${FilteringApp_SOURCES})
set_target_properties(FilteringApp PROPERTIES COMPILE_FLAGS "-std=c++11 -O2 -g -march=native -Wall -Wextra -Werror -pedantic  -Wno-missing-braces ")

#target_link_libraries(FilteringApp -lpcap)

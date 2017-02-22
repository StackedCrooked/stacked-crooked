cmake_minimum_required(VERSION 2.8)
project(FilteringApp)


set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")

set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
include_directories(SYSTEM /usr/local/include /usr/include)

set(FilteringApp_INCLUDES
    Range.h)


set(FilteringApp_SOURCES
    BPFFilter.cpp
    BBInterface.cpp
    BBPort.cpp
    BBServer.cpp
    Logger.cpp
    PhysicalInterface.cpp
    MaskFilter.cpp
    Networking.cpp
    RxTrigger.cpp
    Stack.cpp
    UDPFlow.cpp
    main.cpp)

add_executable(FilteringApp ${FilteringApp_SOURCES})
set_target_properties(FilteringApp PROPERTIES COMPILE_FLAGS "-std=c++14 -O2 -g -march=native -Wall -Wextra -Werror -Wno-missing-braces ")
target_link_libraries(FilteringApp "-L/usr/local/lib -lpcap -pthread")

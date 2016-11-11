cmake_minimum_required(VERSION 2.8)
project(PacketProcessing)


set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")

set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
include_directories(SYSTEM /usr/local/include /usr/include)

set(PacketProcessing_SOURCES
    BPFFilter.cpp
    MaskFilter.cpp
    NativeFilter.cpp
    VectorFilter.cpp
    Networking.cpp
    Packet.cpp
    Utils.cpp
    PCAPWriter.cpp
)

macro(add_target name)
    add_executable(${name} ${PacketProcessing_SOURCES} ${name}.cpp)
    set_target_properties(${name} PROPERTIES COMPILE_FLAGS "-std=c++14 -O2 -march=native -Wall -Wextra -Werror -pedantic -Wno-missing-braces")
    target_link_libraries(${name} -ltbb -lboost_system -lpcap)
endmacro(add_target)


add_target(simple)
add_target(with-hash)

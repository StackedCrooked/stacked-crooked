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


# Helper for _add_target_1
macro(_add_target_3 program filtertype packetsize prefetch)
    add_executable(${program}_${filtertype}_${packetsize}_${prefetch} ${PacketProcessing_SOURCES} ${program}.cpp)
    set_target_properties(${program}_${filtertype}_${packetsize}_${prefetch} PROPERTIES COMPILE_FLAGS "-std=c++14 -O2 -DPREFETCH=${prefetch} -DFILTERTYPE=${filtertype} -DPACKET_SIZE=${packetsize} -march=native -Wall -Wextra -Werror -pedantic -Wno-missing-braces")
    target_link_libraries(${program}_${filtertype}_${packetsize}_${prefetch} -ltbb -lboost_system -lpcap)
endmacro(_add_target_3)


# Helper for add_target
macro(_add_target_2 filtertype packetsize prefetch)
    _add_target_3(simple ${filtertype} ${packetsize} ${prefetch})
    _add_target_3(with-hash ${filtertype} ${packetsize} ${prefetch})
endmacro(_add_target_2)


# Generates a target
macro(_add_target_1 filtertype packetsize)
    _add_target_2(${filtertype} ${packetsize} 0)
    _add_target_2(${filtertype} ${packetsize} 4)
    _add_target_2(${filtertype} ${packetsize} 8)
endmacro(_add_target_1)


# Generates a target
macro(add_target filtertype)
    _add_target_1(${filtertype} 64)
    _add_target_1(${filtertype} 512)
    _add_target_1(${filtertype} 1536)
endmacro(add_target)


add_target(BPFFilter)
add_target(MaskFilter)
add_target(VectorFilter)

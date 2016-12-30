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
macro(_add_target_2 program filtertype prefetch)
    add_executable(${program}_${filtertype}_${prefetch} ${PacketProcessing_SOURCES} ${program}.cpp)
    set_target_properties(${program}_${filtertype}_${prefetch} PROPERTIES COMPILE_FLAGS "-std=c++14 -O2 -march=native -DPREFETCH=${prefetch} -DFILTERTYPE=${filtertype} -Wall -Wextra -Werror -pedantic -Wno-missing-braces")
    target_link_libraries(${program}_${filtertype}_${prefetch} -lpcap)
endmacro(_add_target_2)


# Helper for add_target
macro(_add_target_1 filtertype prefetch)
    _add_target_2(simple ${filtertype} ${prefetch})
    _add_target_2(with-hash ${filtertype} ${prefetch})
endmacro(_add_target_1)


# Generates a target
macro(add_target filtertype)
    _add_target_1(${filtertype} 0)
    _add_target_1(${filtertype} 4)
    _add_target_1(${filtertype} 8)
endmacro(add_target)


add_target(BPFFilter)
add_target(MaskFilter)
add_target(NativeFilter)
add_target(VectorFilter)

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

macro(add_target filtertype prefetch)
    add_executable(simple_${filtertype}_${prefetch} ${PacketProcessing_SOURCES} simple.cpp)
    set_target_properties(simple_${filtertype}_${prefetch} PROPERTIES COMPILE_FLAGS "-std=c++14 -O2 -DPREFETCH=${prefetch} -DFILTERTYPE=${filtertype} -march=native -Wall -Wextra -Werror -pedantic -Wno-missing-braces")
    target_link_libraries(simple_${filtertype}_${prefetch} -ltbb -lboost_system -lpcap)
    add_executable(with-hash_${filtertype}_${prefetch} ${PacketProcessing_SOURCES} with-hash.cpp)
    set_target_properties(with-hash_${filtertype}_${prefetch} PROPERTIES COMPILE_FLAGS "-std=c++14 -O2 -DPREFETCH=${prefetch} -DFILTERTYPE=${filtertype} -march=native -Wall -Wextra -Werror -pedantic -Wno-missing-braces")
    target_link_libraries(with-hash_${filtertype}_${prefetch} -ltbb -lboost_system -lpcap)
endmacro(add_target)


add_target(BPFFilter 0)
add_target(BPFFilter 4)
add_target(BPFFilter 8)
add_target(MaskFilter 0)
add_target(MaskFilter 4)
add_target(MaskFilter 8)
add_target(VectorFilter 0)
add_target(VectorFilter 4)
add_target(VectorFilter 8)

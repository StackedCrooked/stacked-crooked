cmake_minimum_required(VERSION 2.8)
project(PacketProcessing)


set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")

set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
include_directories(SYSTEM /usr/local/include)

set(PacketProcessing_SOURCES
    BPFFilter.cpp
    MaskFilter.cpp
    NativeFilter.cpp
    VectorFilter.cpp
    Networking.cpp
    Utils.cpp
)


add_executable(simple ${PacketProcessing_SOURCES} simple.cpp)
set_target_properties(simple  PROPERTIES COMPILE_FLAGS "-std=c++11 -g -O2 -march=native -Wall -Wextra -Wno-missing-braces -Wno-inconsistent-missing-override")
target_link_libraries(simple  -ltbb -lboost_system -lpcap)


#add_executable(with-hash ${PacketProcessing_SOURCES} with-hash.cpp)
#set_target_properties(with-hash  PROPERTIES COMPILE_FLAGS "-std=c++11 -g -O2 -march=native -Wall -Wextra -Wno-missing-braces -Wno-inconsistent-missing-override")
#target_link_libraries(with-hash -ltbb -lboost_system)

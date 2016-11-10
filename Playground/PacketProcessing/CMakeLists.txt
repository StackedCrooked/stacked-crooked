cmake_minimum_required(VERSION 2.8)
project(PacketProcessing)

set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
set(CXX_FLAGS "${CXX_FLAGS} -std=c++11 ")

include_directories(
    include/protocolstack
    src/testing)

set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")

# The compiler should be invoked via ccache.
set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")

message("CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
include_directories(SYSTEM
    #include/boost
        /usr/local/include # for now, local boost fails in DHCPStateMachine and TCPStateMachine
    include/logging
    include/address2
    include/arch_lib
    include/basetypes
)


set(PacketProcessing_SOURCES
    BPFFilter.cpp
    Networking.cpp
    Utils.cpp
)


add_executable(simple    ${PacketProcessing_SOURCES} simple.cpp   ) 
set_target_properties(simple     PROPERTIES COMPILE_FLAGS "-std=c++11 -g -O2 -march=native -Wall -Wextra -Wno-missing-braces -Wno-inconsistent-missing-override")
target_link_libraries(simple    -ltbb -lboost_system)


#add_executable(with-hash ${PacketProcessing_SOURCES} with-hash.cpp) 
#set_target_properties(with-hash  PROPERTIES COMPILE_FLAGS "-std=c++11 -g -O2 -march=native -Wall -Wextra -Wno-missing-braces -Wno-inconsistent-missing-override")
#target_link_libraries(with-hash -ltbb -lboost_system)

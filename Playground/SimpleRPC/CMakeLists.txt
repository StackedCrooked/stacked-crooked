cmake_minimum_required(VERSION 2.6)
project(SimpleRPC)

set(PROJECT_DIR SimpleRPC_SOURCE_DIR)

include_directories(/opt/local/include)

set(SimpleRPC_SOURCES Networking.cpp RPC.cpp Stopwatch.cpp main.cpp)

add_executable(server ${SimpleRPC_SOURCES})
target_link_libraries(server -L/opt/local/lib -lboost_serialization -lboost_system)
set_target_properties(server PROPERTIES COMPILE_FLAGS "-DRPC_SERVER=1 -Wall -Wextra -Werror -O0 -ggdb")

add_executable(client ${SimpleRPC_SOURCES})
target_link_libraries(client -L/opt/local/lib -lboost_serialization -lboost_system)
set_target_properties(client PROPERTIES COMPILE_FLAGS "-DRPC_CLIENT=1 -Wall -Wextra -Werror -O0 -ggdb")


cmake_minimum_required(VERSION 2.8)
project(SumBenchmark)


set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")

set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
include_directories(SYSTEM /usr/local/include /usr/include)

add_executable(a.out main.cpp)
set_target_properties(a.out PROPERTIES COMPILE_FLAGS "-std=c++11 -O2 -g -march=native -Wall -Wextra -Werror -pedantic -Wno-missing-braces")
target_link_libraries(a.out -lboost_system)

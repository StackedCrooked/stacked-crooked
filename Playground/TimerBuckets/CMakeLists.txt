project(TimerBuckets)
cmake_minimum_required(VERSION 2.8.12)
set(CONAN_SYSTEM_INCLUDES ON)

add_definitions("-std=c++14 -Wall -Wextra -Werror -pedantic-errors")

add_executable(timer_test main.cpp)
target_link_libraries(timer_test)

project(memcpy_vs_cast)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
set_target_properties(${PROJECT_NAME}  PROPERTIES COMPILE_FLAGS " -std=c++0x -ggdb3 -O3 -Wall -Wextra -Werror -pedantic-errors")


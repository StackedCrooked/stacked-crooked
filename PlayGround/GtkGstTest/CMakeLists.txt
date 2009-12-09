cmake_minimum_required (VERSION 2.6) 
project (GtkGstTest)
add_executable (GtkGstTest main.cpp)
include(${CMAKE_ROOT}/Modules/FindPkgConfig.cmake)

# Set CMAKE_C_FLAGS variable with info from pkg-util
execute_process(COMMAND pkg-config --cflags gstreamer-0.10
                OUTPUT_VARIABLE CMAKE_C_FLAGS)
string(REPLACE "\n" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
message("-- CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")

include_directories(/opt/local/include/gtk-2.0)
include_directories(/opt/local/include/cairo)
include_directories(/opt/local/include/pango-1.0)
include_directories(/opt/local/lib/gtk-2.0/include)
include_directories(/opt/local/include/atk-1.0)


# Set CMAKE_EXE_LINKER_FLAGS variable with info from pkg-util
execute_process(COMMAND pkg-config --libs gstreamer-0.10
                OUTPUT_VARIABLE CMAKE_EXE_LINKER_FLAGS)
string(REPLACE "\n" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
message("-- CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

set_target_properties(GtkGstTest
                      PROPERTIES COMPILE_FLAGS ${CMAKE_C_FLAGS}
                                 LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

link_directories(/opt/local/lib) 
target_link_libraries(GtkGstTest gtk-x11-2.0) 

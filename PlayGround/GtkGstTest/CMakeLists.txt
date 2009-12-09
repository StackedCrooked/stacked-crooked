cmake_minimum_required (VERSION 2.6) 
project (GtkGstTest)
add_executable (GtkGstTest main.c)
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
include_directories(/usr/include)
include_directories(/usr/include/gtk-2.0)
include_directories(/usr/include/cairo)
include_directories(/usr/include/pango-1.0)
include_directories(/usr/lib/gtk-2.0/include)
include_directories(/usr/include/atk-1.0)
include_directories(/usr/local/include/gstreamer-0.10)

# Set CMAKE_EXE_LINKER_FLAGS variable with info from pkg-util
execute_process(COMMAND pkg-config --libs gstreamer-0.10
                OUTPUT_VARIABLE CMAKE_EXE_LINKER_FLAGS)
string(REPLACE "\n" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
message("-- CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

set_target_properties(GtkGstTest
                      PROPERTIES COMPILE_FLAGS ${CMAKE_C_FLAGS}
                                 LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

link_directories(/opt/local/lib) 
link_directories(/usr/lib) 
target_link_libraries(GtkGstTest gtk-x11-2.0 X11 gstinterfaces-0.10)

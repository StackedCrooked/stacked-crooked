cmake_minimum_required(VERSION 2.6) 
project(GstSupport)

SET(CMAKE_OSX_ARCHITECTURES "i386")
message("-- CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")

get_filename_component(PROJECT_ROOT "${PROJECT_SOURCE_DIR}/../.." ABSOLUTE CACHE)
message("-- PROJECT_ROOT: ${PROJECT_ROOT}")


include_directories(
    /opt/local/include
    /opt/local/include/gstreamer-0.10
    /opt/local/include/glib-2.0
    /opt/local/lib/glib-2.0/include
    /opt/local/include/libxml2
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(${PROJECT_ROOT}/3rdParty/GStreamer/Sources/libs/gstreamer
                 ${PROJECT_ROOT}/3rdParty/GStreamer/Sources/libs/gst-plugins-base
                 ${PROJECT_ROOT}/3rdParty/GStreamer/Sources/libs/gst-plugins-good
                 ${PROJECT_ROOT}/3rdParty/GStreamer/Sources/libs/gst-plugins-bad
                 /opt/local/lib
)


# Create library.
add_library(
    GstSupport
    SHARED
    src/GstBusListener.cpp
    src/GstMainLoop.cpp
    src/GstSupport.cpp
    src/GstWidget.cpp
)

set_target_properties (GstSupport PROPERTIES COMPILE_FLAGS " -arch i386 -fshort-wchar")
set_target_properties (GstSupport PROPERTIES LINKER_FLAGS " -arch i386")
set_target_properties(GstSupport PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(GstSupport gstreamer-0.10 gobject-2.0 gmodule-2.0 gthread-2.0 xml2 pthread z m glib-2.0 intl iconv gstinterfaces-0.10)


GSTREAMER=/System/Library/Frameworks/GStreamer.framework/Versions/0.10.29-x64
INCLUDE=\
    -I./GstSupport/include \
    -I${GSTREAMER}/include/gstreamer-0.10 \
    -I${GSTREAMER}/include/glib-2.0 \
    -I${GSTREAMER}/lib/glib-2.0/include \
    -I${GSTREAMER}/include/libxml2 \
    -I/opt/local/include \
    -I/usr/include \
    -I.

LIB= \
    -L${GSTREAMER}/lib \
    -lgstreamer-0.10 \
    -lgstvideo-0.10 \
    -lgstinterfaces-0.10 \
    -lgobject-2.0 \
    -lglib-2.0 \
    -L/opt/local/lib -lboost_thread-mt

SRC=main.cpp \
    GstSupport/src/BusListener.cpp \
    GstSupport/src/Logger.cpp \
    GstSupport/src/MainLoop.cpp \
    GstSupport/src/ManualBusListener.cpp \
    GstSupport/src/Support.cpp




all:
	g++ -o test ${INCLUDE} ${LIB} ${SRC}

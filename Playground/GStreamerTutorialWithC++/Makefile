GSTREAMER=/System/Library/Frameworks/GStreamer.framework/Versions/0.10.29-x64
INCLUDE=\
    -I${GSTREAMER}/include/gstreamer-0.10 \
    -I${GSTREAMER}/include/glib-2.0 \
    -I${GSTREAMER}/lib/glib-2.0/include \
    -I${GSTREAMER}/include/libxml2 \
    -I/usr/include

LIB= \
    -L${GSTREAMER}/lib -lgstreamer-0.10 -lgstvideo-0.10 -lgobject-2.0 -lglib-2.0



all:
	gcc -o test ${INCLUDE} ${LIB}  main.c

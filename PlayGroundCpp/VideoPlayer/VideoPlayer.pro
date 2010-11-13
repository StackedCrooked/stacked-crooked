#
# VideoPlayer project
#
QT += core gui


#
# Third-party libraries need to be installed with MacPorts:
# - gstreamer
# - poco
# - boost
#
LIBS += -L/opt/local/lib \
        -lPocoFoundation \
        -lgstreamer-0.10 \
        -lgobject-2.0 \
        -lgmodule-2.0 \
        -lgthread-2.0 \
        -lxml2 \
        -lpthread \
        -lz \
        -lm \
        -lglib-2.0 \
        -lintl \
        -liconv \
        -lgstinterfaces-0.10

INCLUDEPATH += ../GstSupport/include \
               /opt/local/include/ \
               /opt/local/include/gstreamer-0.10 \
               /opt/local/include/glib-2.0 \
               /opt/local/lib/glib-2.0/include \
               /opt/local/include/libxml2


#
# GstSupport
#
HEADERS += \
    ../GstSupport/include/GstSupport/GstBusListener.h \
    ../GstSupport/include/GstSupport/GstMainLoop.h \
    ../GstSupport/include/GstSupport/GstMainLoop.h \
    ../GstSupport/include/GstSupport/GstProbe.h \
    ../GstSupport/include/GstSupport/GstSupport.h \
    ../GstSupport/include/GstSupport/GstTimer.h \
    ../GstSupport/include/GstSupport/GstWidget.h \
    ../GstSupport/include/GstSupport/MakeString.h

SOURCES += \
    ../GstSupport/src/GstBusListener.cpp \
    ../GstSupport/src/GstMainLoop.cpp \
    ../GstSupport/src/GstProbe.cpp \
    ../GstSupport/src/GstSupport.cpp \
    ../GstSupport/src/GstTimer.cpp \
    ../GstSupport/src/GstWidget.cpp


#
# Main application
#
TARGET = VideoPlayer
TEMPLATE = app
SOURCES += \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h

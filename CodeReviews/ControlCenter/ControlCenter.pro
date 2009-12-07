# -------------------------------------------------
# Project created by QtCreator 2009-12-05T15:57:15
# -------------------------------------------------
QT += opengl \
    phonon \
    multimedia
TARGET = ControlCenter
TEMPLATE = app
SOURCES += main.cpp \
    ControlCenterWindow.cpp \
    RTPDataFetcher.cpp \
    ControlCenter.cpp
HEADERS += ControlCenterWindow.h \
    RTPDataFetcher.h \
    ControlCenter.h
INCLUDEPATH += /usr/local/include/jrtplib3
INCLUDEPATH += /usr/local/include/jthread
INCLUDEPATH += /usr/local/Trolltech/Qt-4.6.0/include
LIBS += -L/usr/local/Trolltech/Qt-4.6.0/lib \
    -L/usr/local/lib \
    -ljrtp \
    -ljthread \
    -lPocoFoundation

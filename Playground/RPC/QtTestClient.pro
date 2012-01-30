TEMPLATE = app
CONFIG += console
CONFIG -= qt

LIBS += -lboost_serialization -lboost_system -pthread

SOURCES += \
    Core/TCPServer.cpp \
    Core/TCPClient.cpp \
    Core/RPCServer.cpp \
    Core/Asio.cpp \
    TestClient/TestClient.cpp

HEADERS += \
    Core/TCPServer.h \
    Core/TCPClient.h \
    Core/Serialization.h \
    Core/RPCServer.h \
    Core/RemoteObjects.h \
    Core/RemoteObject.h \
    Core/RemoteCall.h \
    Core/Client.h \
    Core/Asio.h


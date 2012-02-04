TEMPLATE = app
CONFIG += console
CONFIG -= qt

LIBS += -L/opt/local/lib -lboost_serialization -lboost_system -pthread

DEFINES += RPC_SERVER

SOURCES += \
    Core/Asio.cpp \
    Core/TCPServer.cpp \
    Core/TCPClient.cpp \
    Core/RPCServer.cpp \
    TestServer/TestServer.cpp \
    TestServer/Stopwatch.cpp

HEADERS += \
    Core/TCPServer.h \
    Core/TCPClient.h \
    Core/Serialization.h \
    Core/RPCServer.h \
    Core/RemoteObjects.h \
    Core/RemoteObject.h \
    Core/RemoteCall.h \
    Core/Client.h \
    Core/Asio.h \
    TestServer/Stopwatch.h


include(BuildSystem/BuildSystem.pro)

VPATH += $$SOURCES_ROOT/DoIPCLayer/Sources/CPP \
         $$SOURCES_ROOT/DoIPCLayer/Sources/H

INCLUDEPATH += $$VPATH

HEADERS       += DipcConnection.h

SOURCES       += DipcConnection.cpp
QT           += network

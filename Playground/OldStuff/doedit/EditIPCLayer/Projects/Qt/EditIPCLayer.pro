include(BuildSystem/BuildSystem.pro)
include(DoIPCLayer/Projects/Qt/DoIPCLayer.pro)

VPATH += $$SOURCES_ROOT/EditIPCLayer/Sources/CPP \
         $$SOURCES_ROOT/EditIPCLayer/Sources/H

INCLUDEPATH += $$VPATH

HEADERS       += EipcConnection.h

SOURCES       += EipcConnection.cpp

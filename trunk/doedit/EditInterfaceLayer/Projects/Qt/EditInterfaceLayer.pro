include(BuildSystem/BuildSystem.pro)

VPATH += $$SOURCES_ROOT/EditInterfaceLayer/Sources/CPP \
         $$SOURCES_ROOT/EditInterfaceLayer/Sources/H

INCLUDEPATH += $$VPATH

HEADERS       += EiInterface.h

SOURCES       += EiInterface.cpp

include(BuildSystem/BuildSystem.pro)
include(EditViewLayer/Projects/Qt/EditViewLayer.pro)
include(EditIPCLayer/Projects/Qt/EditIPCLayer.pro)
include(EditInterfaceLayer/Projects/Qt/EditInterfaceLayer.pro)

VPATH += $$SOURCES_ROOT/Doedit/Sources/CPP \
         $$SOURCES_ROOT/Doedit/Sources/H

INCLUDEPATH += $$VPATH

#LIBS += qtmaind.lib QtCored.lib QtGuid.lib

win32:QMAKE_LIBDIR += "C:\Program Files\Microsoft Platform SDK\Lib"

SOURCES += main.cpp
CONFIG += qt console
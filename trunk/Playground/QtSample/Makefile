UNAME = $(shell uname)

ifeq ($(UNAME), Linux)
    QT_ROOT = ${HOME}/qt-mingw-2010.05/qt
    MOC = $(QT_ROOT)/bin/moc.exe
    CXX = ${HOME}/qt-mingw-2010.05/mingw/bin/c++.exe
endif


ifeq ($(UNAME), Darwin)
INCLUDE = \
    -F/Library/Frameworks \
    -I/Library/Frameworks/QtGui.framework/Headers \
    -I/Library/Frameworks/QtCore.framework/Headers
endif

ifeq ($(UNAME), Linux)
INCLUDE = \
    -I$(QT_ROOT)/include \
    -I$(QT_ROOT)/include/QtCore \
    -I$(QT_ROOT)/include/QtGui
endif

ifeq ($(UNAME), Darwin)
LIB = \
    -framework QtGui \
    -framework QtCore
endif

ifeq ($(UNAME), Linux)
LIB = \
    -DQT_GUI_LIB -DQT_CORE_LIB -DQT_NO_DEBUG \
    $(QT_ROOT)/bin/QtCore4.dll \
    $(QT_ROOT)/bin/QtGui4.dll
endif

SRC = \
    main.cpp \
    imageviewer.cpp \
    imageviewer_moc.cxx

all: 
	$(MOC) $(INCLUDE) -o imageviewer_moc.cxx imageviewer.h
	$(CXX) $(INCLUDE) $(LIB) -o test $(SRC) -Wl,-rpath,${HOME}/qt-mingw-2010.05/qt/bin


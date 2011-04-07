INCLUDE= \
    -F/Library/Frameworks \
    -I/Library/Frameworks/QtGui.framework/Headers \
    -I/Library/Frameworks/QtCore.framework/Headers

LIB= \
    -framework QtGui \
    -framework QtCore

SRC = \
    main.cpp \
    imageviewer.cpp \
    imageviewer_moc.cxx

all: 
	moc -o imageviewer_moc.cxx imageviewer.h
	g++ $(INCLUDE) $(LIB) -o test $(SRC)


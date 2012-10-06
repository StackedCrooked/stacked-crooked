#-------------------------------------------------
#
# Project created by QtCreator 2012-10-06T18:02:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += /Users/francis/programming/orig/tbb40_20120613oss/include


LIBS += /Users/francis/programming/orig/tbb40_20120613oss/lib/libtbb.dylib

TARGET = tbbvector
TEMPLATE = app


SOURCES += main.cpp\
		MainWindow.cpp

HEADERS  += MainWindow.h

FORMS    += MainWindow.ui

linux-g++-{
	  CFLAGS += -O3 -std=c++0x
	  CXXFLAGS += -O3 -std=c++0x
	  CONFIG += -O3
}

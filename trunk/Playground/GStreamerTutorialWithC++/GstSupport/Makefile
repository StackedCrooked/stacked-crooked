PROJECT_ROOT=..
GSTREAMER_ROOT=/System/Library/Frameworks/GStreamer.framework/Versions/0.10-i386
GSTREAMER_INCLUDE=$(GSTREAMER_ROOT)/include
GSTREAMER_LIB=$(GSTREAMER_ROOT)/lib
GSTSUPPORT_INCLUDE=$(PROJECT_ROOT)/GstSupport/include
GSTSUPPORT_SRC=$(PROJECT_ROOT)/GstSupport/src

CFLAGS= \
    -arch i386 \
    -fshort-wchar \
	-I. \
    -I$(GSTREAMER_INCLUDE) \
    -I$(GSTREAMER_INCLUDE)/glib-2.0 \
    -I$(GSTREAMER_INCLUDE)/gstreamer-0.10 \
    -I$(GSTREAMER_LIB)/glib-2.0/include \
    -I$(GSTREAMER_INCLUDE)/libxml2 \
    -I$(GSTSUPPORT_INCLUDE) \
    -I/opt/local/include

LDFLAGS= \
    -framework AppKit \
    -mmacosx-version-min=10.5 \
    -arch i386 \
    -headerpad_max_install_names \
    -L$(GSTREAMER_ROOT)/lib \
    -L/opt/local/lib \
	-lboost_thread-mt \
    -lgstreamer-0.10 \
    -lgobject-2.0 \
    -lgmodule-2.0 \
    -lgthread-2.0 \
    -lxml2 \
    -lpthread \
    -lz \
    -lm \
    -lglib-2.0 \
    -lintl \
    -liconv \
    -lgstinterfaces-0.10 \

SOURCES_GST= \
	BusListener.cpp \
	Logger.cpp \
	MacWidget.cpp \
	MainLoop.cpp \
	NativeWindow.cpp \
	Probe.cpp \
	Support.cpp \
	Timer.cpp \
	VideoPlayer.cpp \
	Widget.cpp \
	XWidget.cpp \
	main.cpp

SOURCES=$(foreach file,$(SOURCES_GST),$(GSTSUPPORT_SRC)/$(file))

OBJECTS=$(SOURCES:.cpp=.o)
TARGET=run-testsuite

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	g++ -c $(CFLAGS) src/MacOSXWindow.mm -o src/MacOSXWindow.o
	g++ $(LDFLAGS) $(OBJECTS) src/MacOSXWindow.o -o $@

.cpp.o:
	gcc -c $(CFLAGS) $< -o $@

clean:
	find . -name "*.o" | xargs rm -f $(OBJECTS)


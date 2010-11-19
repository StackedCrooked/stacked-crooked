XULRUNNER_INCLUDE= /usr/include/xulrunner-1.9.2.12
XULRUNNER_LIB=/usr/lib/xulrunner-1.9.2.12
XULRUNNER_SDK_LIB=/usr/lib/xulrunner-devel-1.9.2.12/sdk/lib
REGXPCOM=/home/francis/svn/gst-media-player/3rdParty/XULRunnerSDK/xulrunner-sdk/sdk/bin/regxpcom
XULRUNNER_BIN=/usr/lib/xulrunner-1.9.2.12
XULRUNNER_COMPONENTS=/usr/lib/xulrunner-1.9.2.12/components
XULRUNNER_SHARE=/usr/share/idl/xulrunner-1.9.2.12
INCLUDE_DIRS = -I$(XULRUNNER_INCLUDE) -I/usr/include/nspr
LNK_DIRS = -L$(XULRUNNER_LIB) -L$(XULRUNNER_SDK_LIB) -L$(XULRUNNER_BIN) -L/usr/lib
LNK_FLAGS= $(LNK_DIRS) -Wl,-rpath-link=$(XULRUNNER_BIN) -lxpcomglue_s -lxpcom -lnspr4 
CPP_FLAGS = -DXP_UNIX -D_IMPL_NS_COM -D_BSD_SOURCE


all: xpt so

MyComponentModule:
	g++ -w -c -o MyComponentModule.o $(INCLUDE_DIRS) $(CPP_FLAGS) MyComponentModule.cpp

MyComponent:
	g++ -w -c -o MyComponent.o $(INCLUDE_DIRS) $(CPP_FLAGS) MyComponent.cpp

impl: MyComponent MyComponentModule

so: impl
	g++ -shared -o MyComponent.so *.o $(LNK_FLAGS)

xpt:
	$(XULRUNNER_BIN)/xpidl -m header -I$(XULRUNNER_SHARE) IMyComponent.idl
	$(XULRUNNER_BIN)/xpidl -m typelib -I$(XULRUNNER_SHARE) IMyComponent.idl

install:
	cp MyComponent.so $(XULRUNNER_COMPONENTS)
	cp IMyComponent.xpt $(XULRUNNER_COMPONENTS)
	$(REGXPCOM) -x $(XULRUNNER_COMPONENTS)
	$(REGXPCOM) -x $(XULRUNNER_BIN) -a

uninstall:
	rm -f $(XULRUNNER_COMPONENTS)/MyComponent.so
	rm -f $(XULRUNNER_COMPONENTS)/IMyComponent.xpt

clean:
	rm -f *.o
	rm -f *.xpt
	rm -f MyComponent.so
	rm -f IMyComponent.h

mozclean:
	rm -f $(XULRUNNER_COMPONENTS)/compreg.dat
	rm -f $(XULRUNNER_COMPONENTS)/xpti.dat
	rm -rf ~/.mozilla
	rm -rf ~/.testapplicationsinc


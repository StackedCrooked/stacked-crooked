TARGET=MyComponent.so
CXX=c++
XULRUNNER=$(HOME)/xulrunner-sdk
XULRUNNER_BIN=$(XULRUNNER)/bin
XULRUNNER_COMPONENTS=$(XULRUNNER_BIN)/components
XULRUNNER_IDL=$(XULRUNNER)/idl
XULRUNNER_INCLUDE=$(XULRUNNER)/include
XULRUNNER_LIB=$(XULRUNNER)/lib
XULRUNNER_SDK_LIB=$(XULRUNNER)/sdk/lib
INCLUDES=-include "xpcom-config.h" -I$(XULRUNNER_INCLUDE) -I/usr/include/nspr
REGXPCOM=$(XULRUNNER)/sdk/bin/regxpcom

# Link flags 'exactly as stated' in https://developer.mozilla.org/en/XPCOM_Glue
LD_FLAGS+= -L$(XULRUNNER_LIB) -L$(XULRUNNER_BIN) \
           -Wl,-z,defs \
           -Wl,-rpath-link,$(XULRUNNER_BIN) \
           -lxpcomglue_s -lxpcom -lnspr4

CPP_FLAGS+= -fPIC -shared


all: xpt src

src:
	$(CXX) $(CPP_FLAGS) -c MyComponent.cpp $(INCLUDES)
	$(CXX) $(CPP_FLAGS) -c MyComponentModule.cpp $(INCLUDES)
	$(CXX) -shared -o $(TARGET) *.o $(LD_FLAGS)
	chmod 755 $(TARGET)
	strip $(TARGET)

xpt:
	$(XULRUNNER_BIN)/xpidl -m header -I$(XULRUNNER_IDL) IMyComponent.idl
	$(XULRUNNER_BIN)/xpidl -m typelib -I$(XULRUNNER_IDL) IMyComponent.idl

install:
	cp MyComponent.so $(XULRUNNER_COMPONENTS)/
	cp IMyComponent.xpt $(XULRUNNER_COMPONENTS)/
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


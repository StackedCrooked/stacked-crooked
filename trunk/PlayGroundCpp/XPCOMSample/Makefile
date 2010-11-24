TARGET=MyComponent.so
CXX=c++
XULRUNNER=/home/francis/opt/XULRunnerSDK/1.9.1/xulrunner-sdk
INCLUDES=-include "xpcom-config.h" -I$(XULRUNNER)/sdk/include -I$(XULRUNNER)/include -I/usr/include/nspr
REGXPCOM=$(XULRUNNER)/sdk/bin/regxpcom

LD_FLAGS+= -L$(XULRUNNER)/lib -L$(XULRUNNER)/bin \
           -Wl,-z,defs \
           -Wl,-rpath-link,$(XULRUNNER)/bin \
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
	$(XULRUNNER)/sdk/bin/xpidl -m header -I$(XULRUNNER)/idl IMyComponent.idl
	$(XULRUNNER)/sdk/bin/xpidl -m typelib -I$(XULRUNNER)/idl IMyComponent.idl

install:
	cp MyComponent.so $(XULRUNNER)/bin/components
	cp IMyComponent.xpt $(XULRUNNER)/bin/components
	$(REGXPCOM) -x $(XULRUNNER)/bin -a

uninstall:
	rm -f $(XULRUNNER)/bin/components/MyComponent.so
	rm -f $(XULRUNNER)/bin/components/IMyComponent.xpt

clean:
	rm -f *.o
	rm -f *.xpt
	rm -f MyComponent.so
	rm -f IMyComponent.h

mozclean:
	rm -f $(XULRUNNER)/bin/components/compreg.dat
	rm -f $(XULRUNNER)/bin/components/xpti.dat
	rm -rf ~/.mozilla
	rm -rf ~/.testapplicationsinc


# Configuration
# -------------
XULRUNNER_ROOT=$(HOME)/opt/XULRunnerSDK/1.9.1/xulrunner-sdk


# Makefile
# --------
TARGET=MyComponent.so
CXX=c++
XULRUNNER_SDK=$(XULRUNNER_ROOT)/sdk
INCLUDES=-I$(XULRUNNER_ROOT)/include -I$(XULRUNNER_SDK)/include
REGXPCOM=$(XULRUNNER_SDK)/bin/regxpcom

LD_FLAGS+= -L$(XULRUNNER_SDK)/lib -L$(XULRUNNER_SDK)/bin \
           -Wl,-z,defs \
           -Wl,-rpath-link,$(XULRUNNER_ROOT)/bin \
           -lxpcomglue_s -lxpcom -lnspr4

CPP_FLAGS+= -m32 -fno-rtti -fno-exceptions -fPIC -shared
 
all: xpt src

src:
	$(CXX) $(CPP_FLAGS) -c MyComponent.cpp $(INCLUDES)
	$(CXX) $(CPP_FLAGS) -c MyComponentModule.cpp $(INCLUDES)
	$(CXX) -m32 -shared -o $(TARGET) *.o $(LD_FLAGS)
	chmod 755 $(TARGET)
	strip $(TARGET)

xpt:
	$(XULRUNNER_SDK)/bin/xpidl -m header -I$(XULRUNNER_ROOT)/idl IMyComponent.idl
	$(XULRUNNER_SDK)/bin/xpidl -m typelib -I$(XULRUNNER_ROOT)/idl IMyComponent.idl

install:
	ln -fs $(XULRUNNER_ROOT)/bin/xulrunner ~/bin/xulrunner
	cp MyComponent.so $(XULRUNNER_ROOT)/bin/components
	cp IMyComponent.xpt $(XULRUNNER_ROOT)/bin/components
	$(REGXPCOM) -x $(XULRUNNER_ROOT)/bin -a

uninstall:
	rm -f $(XULRUNNER_ROOT)/bin/components/MyComponent.so
	rm -f $(XULRUNNER_ROOT)/bin/components/IMyComponent.xpt

clean:
	rm -f *.o
	rm -f *.xpt
	rm -f MyComponent.so
	rm -f IMyComponent.h

mozclean:
	rm -f $(XULRUNNER_ROOT)/bin/components/compreg.dat
	rm -f $(XULRUNNER_ROOT)/bin/components/xpti.dat
	rm -rf ~/.mozilla

supermake:
	sudo make uninstall ; sudo make mozclean ; make clean ; make ; sudo make install


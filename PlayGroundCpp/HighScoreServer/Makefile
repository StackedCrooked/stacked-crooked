CPP_DEFINES=
INCLUDE_DIRS=-Iinclude
LIBS= \
    -lPocoFoundation \
	-lPocoNet \
	-lPocoXML \
	-lPocoUtil \
	-lPocoData \
	-lPocoSQLite

all:
	g++ -o HighScoreServer $(INCLUDE_DIRS) $(CPP_DEFINES) $(LIBS) src/*.cpp

clean:
	rm -f *.o

install:
	cp -f ./HighScoreServer /usr/bin/
	cp -f ./scripts/hsserv /etc/init.d/

start:
	/etc/init.d/hsserv start

stop:
	/etc/init.d/hsserv stop

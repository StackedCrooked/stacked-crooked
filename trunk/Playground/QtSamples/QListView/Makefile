QTSDK=/Users/francis/QtSDK
MOC=${QTSDK}/Desktop/Qt/4.8.1/gcc/bin/moc 
INCLUDE= -I/opt/local/include -I/usr/local/include 
CXX_FLAGS=-std=c++0x -g -O3 -Wall -Werror -Wextra ${INCLUDE} -L/opt/local/lib -L/usr/local/lib -lPocoFoundation -lPocoNet -lPocoZip -lPocoUtil -isystem ${QTSDK}/Desktop/Qt/4.8.1/gcc/include -F${QTSDK}/Desktop/Qt/4.8.1/gcc/lib -framework QtGui -framework QtCore -DQT_NO_KEYWORDS -DQT_NO_DEBUG -DQT_NO_STL

all:
	rm -f *.o
	${MOC} ${INCLUDE} -o moc_MainWindow.cxx MainWindow.h
	g++ -c  ${CXX_FLAGS} moc_MainWindow.cxx
	g++ -c  ${CXX_FLAGS} ListView.cpp
	g++ -c  ${CXX_FLAGS} MainWindow.cpp
	g++ -c  ${CXX_FLAGS} main.cpp
	g++ -c  ${CXX_FLAGS} Importer.cpp
	g++ -o test *.o ${CXX_FLAGS}

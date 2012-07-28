QTSDK=/Users/francis/QtSDK
MOC=${QTSDK}/Desktop/Qt/4.8.0/gcc/bin/moc 
INCLUDE= -I/opt/local/include -I/usr/local/include 
CXX_FLAGS=-std=c++0x -g -O0 -Wall -Werror -Wextra ${INCLUDE} -L/opt/local/lib -L/usr/local/lib -lPocoFoundation -lPocoNet -lPocoZip -lPocoUtil -isystem ${QTSDK}/Desktop/Qt/4.8.0/gcc/include -F${QTSDK}/Desktop/Qt/4.8.0/gcc/lib -framework QtGui -framework QtCore

all:
	rm -f *.o
	${MOC} ${INCLUDE} -o moc_ImageViewer.cxx ImageViewer.h
	g++ -c  ${CXX_FLAGS} moc_ImageViewer.cxx
	g++ -c  ${CXX_FLAGS} main.cpp
	g++ -c  ${CXX_FLAGS} ImageViewer.cpp
	g++ -o test *.o ${CXX_FLAGS}

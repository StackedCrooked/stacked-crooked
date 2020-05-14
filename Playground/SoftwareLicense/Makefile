all:
	protoc --cpp_out=Protobuf license.proto
	g++ -std=c++17 -g -Wall -pedantic -pthread -lcryptopp main.cpp Protobuf/license.pb.cc /usr/local/lib/libprotobuf.a


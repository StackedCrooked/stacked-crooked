all:
	protoc --cpp_out=Protobuf license.proto
	g++ -std=c++17 -g -Wall -pedantic -pthread -lcryptopp main.cpp Protobuf/license.pb.cc -lboost_program_options -lprotobuf -o license-tool


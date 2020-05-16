all:
	protoc --cpp_out=Protobuf license.proto
	g++ -std=c++17 -g -Wall -pedantic Protobuf/license.pb.cc main.cpp -pthread -lcrypto++ -lboost_program_options -lprotobuf -o license-tool


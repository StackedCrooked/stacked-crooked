all:
	mkdir -p Protobuf
	protoc --cpp_out=Protobuf license.proto
	g++ -std=c++17 -O2 -Wall -pedantic Protobuf/license.pb.cc main.cpp -o license-tool -pthread -lboost_program_options-mt -lprotobuf
	#-lcrypto++ 


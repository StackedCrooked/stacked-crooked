all:
	mkdir -p Protobuf
	protoc --cpp_out=Protobuf license.proto
	g++ -std=c++17 -g -Wall -pedantic Protobuf/license.pb.cc version1.cpp -pthread -lcrypto++ -lboost_program_options -lprotobuf -o license-tool-v1
	g++ -std=c++17 -g -Wall -pedantic Protobuf/license.pb.cc version2.cpp -pthread -lcrypto++ -lboost_program_options -lprotobuf -o license-tool-v2
	g++ -std=c++17 -g -Wall -pedantic Protobuf/license.pb.cc version3.cpp -pthread -lcrypto++ -lboost_program_options -lprotobuf -o license-tool-v3
	g++ -std=c++17 -g -Wall -pedantic Protobuf/license.pb.cc main.cpp -pthread -lcrypto++ -lboost_program_options -lprotobuf -o license-tool


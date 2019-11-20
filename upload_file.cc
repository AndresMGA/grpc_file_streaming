#include <iostream>
#include <string>
#include <fstream>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "file_streaming.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using routeguide::FileStreaming;
using routeguide::StreamingError;
using routeguide::FileToStream;
using namespace std;

class FileStreamingClient {
 public:
	FileStreamingClient(std::shared_ptr<Channel> channel): stub_(FileStreaming::NewStub(channel)) {}

    void uploadFile(char* file_name) {
		
		FileToStream fu;
		ClientContext context;
		StreamingError se;
		
		fu.set_name(file_name);
 
		//open and read requested filefile 
    	ifstream file;
		char * memblock;
		int size;
		file.open ("client_storage/" + fu.name(), ios::out|ios::binary|ios::ate);
		if (file.is_open())
		{
			size = file.tellg();
			memblock = new char [size];
			file.seekg (0, ios::beg);
			file.read (memblock, size);
			file.close();
    
			fu.set_binary(memblock);
			fu.set_byte_count(size);
 		}
		else 
		{
			std::cout << "Couldn't open file." << std::endl;
			return;
		}
	
	
		//stream data to server
		std::unique_ptr<ClientWriter<FileToStream> > writer(stub_->uploadFile(&context, &se));
		if (!writer->Write(fu)) 
		{
			std::cout << "Broken stream." << std::endl;
		}
		writer->WritesDone();
		Status status = writer->Finish();
		
		if (status.ok()) 
		{
			std::cout << "RPC File Upload succeeded." << std::endl;
		} 
		else 
		{
			std::cout << "RPC File Upload failed." << std::endl;
		}
	}

 private:
	std::unique_ptr<FileStreaming::Stub> stub_;
};

int main(int argc, char** argv) {

	//create channel
	FileStreamingClient fschan(grpc::CreateChannel("localhost:50051",grpc::InsecureChannelCredentials()));
	
	std::cout << "-------------- Uploading File --------------" << std::endl;
	fschan.uploadFile(argv[1]);
	return 0;
}

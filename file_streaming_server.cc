#include <iostream>
#include <string>
#include <fstream>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "file_streaming.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using routeguide::FileStreaming;
using routeguide::TargetFileName;
using routeguide::StreamingError;
using routeguide::FileToStream;
using namespace std;

class FileStreamingImpl final : public FileStreaming::Service {
 public:
	explicit FileStreamingImpl(){}
	
	Status downloadFile(ServerContext* context,const TargetFileName* target_file,ServerWriter<FileToStream>* writer) override 
	{
		FileToStream f;
		
		//open and read required file
		ifstream file;
		char * memblock;
		int size;
		file.open ("server_storage/" + target_file->name(), ios::in|ios::binary|ios::ate);
		if (file.is_open())
		{
			size = file.tellg();
			memblock = new char [size];
			file.seekg (0, ios::beg);
			file.read (memblock, size);
			file.close();
			
			//send data to client
			f.set_binary(memblock);
			f.set_byte_count(size);
			f.set_name(target_file->name());
			writer->Write(f);
			
			delete[] memblock;
			return Status::OK;
		}
 		else std::cout << "Unable to open file"<< std::endl;
		return Status::CANCELLED;
  }
  
	Status uploadFile(ServerContext* context, ServerReader<FileToStream>* reader, StreamingError* se) override 
	{	
		FileToStream fu;

		//read data from client
		while(reader->Read(&fu))
		{
			std::cout << "Uploading File --- " << fu.name() << "\n" 
					  << fu.binary() << "\n"   
					  << fu.byte_count() << " bytes uploaded " << std::endl;
		}
    
		//store data/file in server storage
		ofstream file;
		int size = fu.byte_count();
		file.open ("server_storage/" + fu.name(), ios::out|ios::binary|ios::ate);
		if (file.is_open())
		{
			file.seekp (0, ios::beg);
			file.write (fu.binary().c_str(),size );
			file.close();
			return Status::OK;
		}
		else std::cout << "Unable to open file"<< std::endl;
    
		return Status::CANCELLED;
  }
 private:
};

int main(int argc, char** argv) {
	
	//start the server
	std::string server_address("0.0.0.0:50051");
	FileStreamingImpl service;
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
	return 0;
}

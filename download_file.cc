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
using routeguide::TargetFileName;
using routeguide::FileToStream;
using namespace std;

class FileStreamingClient {
 public:
	FileStreamingClient(std::shared_ptr<Channel> channel): stub_(FileStreaming::NewStub(channel)) {}
	void downloadFile(char* file_name) {
	 
		TargetFileName tf;
		FileToStream fd;
		ClientContext context;
		tf.set_name(file_name); 
		//fetch file from server

		std::unique_ptr<ClientReader<FileToStream> > reader(stub_->downloadFile(&context, tf));
		while(reader->Read(&fd))
		{
			std::cout << "Downloading File --- " << fd.name() << "\n" 
					  << fd.binary() << "\n" 
					  << fd.byte_count() << " bytes downloaded " << std::endl;
		}
		Status status = reader->Finish();
		if (status.ok()) 
		{
			std::cout << "RPC File Download succeeded." << std::endl;
		} 
		else 
		{
			std::cout << "RPC File Download failed." << std::endl;
		}
    
		//store file in client
		ofstream file;
		int size = fd.byte_count();
		file.open ("client_storage/" + fd.name(), ios::out|ios::binary|ios::ate);
		if (file.is_open())
		{
			file.seekp (0, ios::beg);
			file.write (fd.binary().c_str(),size );
			file.close();
		}
		else std::cout << "Unable to open file"<< std::endl;
	}
 private:
	std::unique_ptr<FileStreaming::Stub> stub_;
};

int main(int argc, char** argv) {

	//create channel
	FileStreamingClient fschan(grpc::CreateChannel("localhost:50051",grpc::InsecureChannelCredentials()));

	std::cout << "-------------- Downloading File --------------" << std::endl;
	fschan.downloadFile(argv[1]);
	return 0;
}

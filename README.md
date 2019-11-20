# gRPC File Streaming C++ example

## usage:

Add executable permissions to **file_streaming_server** , **upload_file** , **download_file**

```
sudo chmod +x file_streaming_server upload_file download_file 
```

### start server

```
./file_streaming_server
```
#### open another teminal 

##### to upload a file 
```
./upload_file [file name]
```
(client will look for a the file in ./client_storage and store it in ./server_storage)

##### to download a file 
```
./download_file [file name]  
```
(client will look for a the file in ./server_storage and store it in ./client_storage)



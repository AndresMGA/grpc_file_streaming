# gRPC File Streaming C++ example

##usage:

### start server

```
./file_straming_server
```
###open another teminal 

####to upload a file 
```
./upload_file [file name]
```
(client will look for a the file in ./client_storage and store it in ./server_storage)

####to download a file 
```
./download_file [file name]  
```
(client will look for a the file in ./server_storage and store it in ./client_storage)



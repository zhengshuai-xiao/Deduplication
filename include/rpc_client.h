#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "include/storageserver.grpc.pb.h"
#include "include/common.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using storageserver::Storage;
using storageserver::HelloReply;
using storageserver::HelloRequest;

using storageserver::FileWriteRequest;
using storageserver::FileWriteReply;
using storageserver::FileMetaDataRequest;
using storageserver::FileMetaDataReply;

class StorageClient {
public:
    StorageClient(std::shared_ptr<Channel> channel)
        : stub_(Storage::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string& user);
    int FileOpen(const std::string path, int mode);
    size_t FileWrite(const std::string path, const char* data, size_t size);

    int FileClose(const std::string path);

 private:
  std::unique_ptr<Storage::Stub> stub_;
};
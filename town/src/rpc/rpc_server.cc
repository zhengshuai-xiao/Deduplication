#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "include/storageserver.grpc.pb.h"
#include "include/common.h"

using grpc::CallbackServerContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerUnaryReactor;
using grpc::Status;

using storageserver::Storage;
using storageserver::HelloReply;
using storageserver::HelloRequest;

using storageserver::FileWriteRequest;
using storageserver::FileWriteReply;
using storageserver::FileMetaDataRequest;
using storageserver::FileMetaDataReply;

// Logic and data behind the server's behavior.
class StorageServiceImpl final : public Storage::CallbackService {
  ServerUnaryReactor* SayHello(CallbackServerContext* context,
                               const HelloRequest* request,
                               HelloReply* reply) override {
    FUNC_TRACE();
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());

    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
  }

  ServerUnaryReactor* FileOpen(CallbackServerContext* context,
                               const FileMetaDataRequest* request,
                               FileMetaDataReply* reply) override {
    FUNC_TRACE();
    reply->set_id(request->id());
    reply->set_fullpath(request->fullpath());
    LOG_INFO() << "FileOpen: " << request->id() << ", " << request->fullpath();

    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
  }

  ServerUnaryReactor* FileWrite(CallbackServerContext* context,
                               const FileWriteRequest* request,
                               FileWriteReply* reply) override {
    FUNC_TRACE();
    reply->set_id(request->id());
    reply->set_fullpath(request->fullpath());
    LOG_TRACE() << "FileWrite: " << request->id() << ", " << request->fullpath() << ", " << request->len();

    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
  }

  ServerUnaryReactor* FileClose(CallbackServerContext* context,
                               const FileMetaDataRequest* request,
                               FileMetaDataReply* reply) override {
    FUNC_TRACE();
    reply->set_id(request->id());
    reply->set_fullpath(request->fullpath());

    LOG_INFO() << "FileClose: " << request->id() << ", " << request->fullpath();

    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
  }

};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    StorageServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    LOG_INFO() << "Server listening on " << server_address;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}



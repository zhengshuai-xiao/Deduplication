#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "storageserver.grpc.pb.h"
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
    std::string SayHello(const std::string& user) {
        // Data we are sending to the server.
        HelloRequest request;
        request.set_name(user);

        // Container for the data we expect from the server.
        HelloReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        std::mutex mu;
        std::condition_variable cv;
        bool done = false;
        Status status;
        stub_->async()->SayHello(&context, &request, &reply,
                                [&mu, &cv, &done, &status](Status s) {
                                status = std::move(s);
                                std::lock_guard<std::mutex> lock(mu);
                                done = true;
                                cv.notify_one();
                                });

        std::unique_lock<std::mutex> lock(mu);
        while (!done) {
        cv.wait(lock);
        }

        // Act upon its status.
        if (status.ok()) {
        return reply.message();
        } else {
        std::cout << status.error_code() << ": " << status.error_message()
                    << std::endl;
        return "RPC failed";
        }
    }

 private:
  std::unique_ptr<Storage::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target="
                  << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  StorageClient client(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  std::string user("world");
  std::string reply = client.SayHello(user);
  std::cout << "StorageClient received: " << reply << std::endl;

  return 0;
}
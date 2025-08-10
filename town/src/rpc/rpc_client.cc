#include "include/rpc_client.h"

// Assembles the client's payload, sends it and presents the response back
// from the server.
std::string StorageClient::SayHello(const std::string& user) {
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
        LOG_ERROR() << status.error_code() << ": " << status.error_message();
        return "RPC failed";
    }
}
int StorageClient::FileOpen(const std::string path, int mode){
    FileMetaDataRequest request;
    request.set_fullpath(path);
    FileMetaDataReply reply;
    ClientContext context;
    Status status = stub_->FileOpen(&context, request, &reply);
    if (status.ok()) {
        LOG_TRACE() << "FileOpen succeeded: " << reply.fullpath();
        return 0;
    } else {
        LOG_ERROR() << "FileOpen failed: " << status.error_message();
        return -1;
    }
}
size_t StorageClient::FileWrite(const std::string path, const char* data, size_t size){
    FileWriteRequest request;
    request.set_len(size);
    request.set_payload(data);
    request.set_fullpath(path);

    FileWriteReply reply;
    ClientContext context;

    Status status = stub_->FileWrite(&context, request, &reply);
    if (status.ok()) {
        LOG_TRACE() << "FileWrite succeeded: " << reply.fullpath();
    } else {
        LOG_ERROR() << "FileWrite failed: " << status.error_message();
    }
    return reply.len();
}

int StorageClient::FileClose(const std::string path){
    FileMetaDataRequest request;
    request.set_fullpath(path);
    FileMetaDataReply reply;
    ClientContext context;
    Status status = stub_->FileClose(&context, request, &reply);
    if (status.ok()) {
        LOG_TRACE() << "FileClose succeeded: " << reply.fullpath();
        return 0;
    } else {
        LOG_ERROR() << "FileClose failed: " << status.error_message();
        return -1;
    }
}
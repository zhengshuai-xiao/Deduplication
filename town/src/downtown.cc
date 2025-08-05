#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "include/common.h"
#include "rpc/rpc_server.h"

int down_town_main(int argc, char** argv) {

    LOG_INFO() << "Starting downtown server...";
    std::thread server_thread(RunServer);
    //TODO: join other tasks
    
    server_thread.join();
    LOG_INFO() << "Downtown server stopped.";
    return 0;
}

int main(int argc, char** argv) {
    //TODO:deamon?
    down_town_main(argc, argv);
    return 0;
}
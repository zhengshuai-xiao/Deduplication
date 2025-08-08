#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "include/common.h"
#include "rpc/rpc_server.h"
#include "downtown.h"

server_config server_cfg;

void config_init(const std::string cfg_path) {
    FUNC_TRACE();
    ConfigManager::Instance().LoadFromJson(cfg_path);
    server_cfg.address = ConfigManager::Instance().Get<std::string>("server.address", "0.0.0.0:50051");
    server_cfg.timeout = ConfigManager::Instance().Get<int>("server.timeout", 30);
    server_cfg.log_level = ConfigManager::Instance().Get<std::string>("log.level", "info");
    server_cfg.log_path = ConfigManager::Instance().Get<std::string>("log.path", "/var/log/downtown.log");
}

int down_town_main(const std::string cfg_path) {
    FUNC_TRACE();
    config_init(cfg_path);
    //init logger
    common::Logger::Instance().setLogFile(server_cfg.log_path, true);
    common::Logger::Instance().setLevel(server_cfg.log_level);

    LOG_INFO() << "Starting downtown server...";
    std::thread server_thread(RunServer, server_cfg.address);
    //TODO: join other tasks
    
    server_thread.join();
    LOG_INFO() << "Downtown server stopped.";

    common::Logger::Instance().stop();//stop logging thread
    return 0;
}

int main(int argc, char** argv) {
    //parse the command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string cfg_path = argv[1];
    //TODO:deamon?
    down_town_main(cfg_path);
    return 0;
}
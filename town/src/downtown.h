#pragma once
#include <string>

struct server_config {
    std::string address;
    int timeout;
    std::string log_level;
    std::string log_path;
};

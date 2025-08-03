#pragma once
#include <iostream>
#include <memory>
#include <string>

class mockFileOps
{
private:

public:
    mockFileOps(/* args */);
    ~mockFileOps();
    int open(std::string filename,);
    int write(int fd, const char* data, size_t size);
    int read(int fd, char* buffer, size_t size);
};

mockFileOps::mockFileOps(/* args */)
{
}

mockFileOps::~mockFileOps()
{
}

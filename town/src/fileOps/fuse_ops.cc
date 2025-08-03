#include "fuse_ops.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>

std::unordered_map<int,int> file_descriptors;

int fs_open(const char *path, struct fuse_file_info *fi){
    return 0;
}

// 读取文件内容
int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    return 0;
}

// 写入文件内容
int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    return 0;
}

// 释放文件描述符
int fs_release(const char *path, struct fuse_file_info *fi){
    return 0;
}
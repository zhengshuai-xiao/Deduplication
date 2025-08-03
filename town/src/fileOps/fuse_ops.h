#pragma once
#include "fuse_define.h"

int fs_open(const char *path, struct fuse_file_info *fi);

// 读取文件内容
int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

// 写入文件内容
int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

// 释放文件描述符
int fs_release(const char *path, struct fuse_file_info *fi);

// 截断文件大小
//int my_truncate(const char *path, off_t size);

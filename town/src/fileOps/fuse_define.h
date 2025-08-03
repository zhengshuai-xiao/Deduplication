#pragma once
#define FUSE_USE_VERSION 30
#include <fuse.h>

static struct fuse_operations myfs_oper = {
    .getattr    = myfs_getattr,
    .readdir    = myfs_readdir,
    .open       = myfs_open,
    .read       = myfs_read,
    .write      = myfs_write,
    .create     = myfs_create,
    .unlink     = myfs_unlink,
    .mkdir      = myfs_mkdir,
    .rmdir      = myfs_rmdir,
    .rename     = myfs_rename,
    .truncate   = myfs_truncate,
    .chmod      = myfs_chmod,
    .release    = myfs_release,
    // TODO:add more
};
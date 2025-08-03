#pragma once
#include <iostream>
typedef union fp{
    char bytes[16];
    uint32_t uint[4];
}fp_t;

#define MAX_TRUNKS 100
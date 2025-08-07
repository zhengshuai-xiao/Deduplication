#pragma once
#include "define.h"
#include "buffer.h"

class SegBuffer:public Buffer{
    public:
        SegBuffer(size_t size):Buffer(size){};
        virtual ~SegBuffer() = default;
        void calcFP();
        void getFP(fp_t* fp);
    private:
        fp_t fp_;
};

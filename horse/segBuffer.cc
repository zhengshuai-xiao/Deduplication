#include "segBuffer.h"

void SegBuffer::calcFP(){
    //TODO:
}

void SegBuffer::getFP(fp_t* fp){
    memcpy(fp->bytes,fp_.bytes, sizeof(fp_.bytes));
}

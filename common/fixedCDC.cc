#pragma

#include "FixedCDC.h"

int FixedCDC::calc_chunks(
  const char* const data,
  const size_t size,
  uint64_t &chunk_offset) const
{
  if (size==0) {
    return -1;
  }

  chunk_offset = min(size, chunk_size);
  
  return chunk_offset==size? -1:0;
}
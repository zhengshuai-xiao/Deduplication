#pragma

#include "fixedCDC.h"

int FixedCDC::calc_chunks(
  const char* const data,
  const size_t size,
  uint64_t &chunk_size)
{
  if (size==0) {
    chunk_size=0;
    return -1;
  }

  chunk_size = std::min(size, chunk_size_);

  return chunk_size == size ? -1 : 0;
}
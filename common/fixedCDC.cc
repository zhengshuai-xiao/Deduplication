#pragma

#include "FixedCDC.h"

void FixedCDC::calc_chunks(
  const bufferlist& bl,
  std::vector<std::pair<uint64_t, uint64_t>> *chunks) const
{
  size_t len = bl.length();
  if (!len) {
    return;
  }
  for (size_t pos = 0; pos < len; pos += chunk_size) {
    chunks->push_back(std::pair<uint64_t,uint64_t>(pos, std::min(chunk_size,
								 len - pos)));
  }
}
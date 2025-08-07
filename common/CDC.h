#pragma once

#include <vector>
#include <string>
#include <memory>

#include "buffer.h"

class CDC {
public:
  virtual ~CDC() = default;

  /* calculate chunk boundaries
     0: did not hit the buffer boundary
     -1: hit the buffer boundary
  */
  virtual int calc_chunks(const char* const data,
  const size_t size,
  uint64_t &chunk_offset) = 0;

  /// set target chunk size as a power of 2, and number of bits for hard min/max
  virtual void set_target_bits(int bits, int windowbits = 2) = 0;

  static std::unique_ptr<CDC> create(
    const std::string& type,
    int bits,
    int windowbits = 0);
};

//void generate_buffer(int size, bufferlist *outbl, int seed = 0);

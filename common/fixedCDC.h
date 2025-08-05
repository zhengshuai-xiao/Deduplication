#pragma once

#include "CDC.h"

class FixedCDC : public CDC {
private:
  size_t chunk_size;

public:
  FixedCDC(int target = 17, int window_bits = 0) {
    set_target_bits(target, window_bits);
  };

  void set_target_bits(int target, int window_bits) override {
    chunk_size = 1ul << target;
  }
  int calc_chunks(const char* const data,
  const size_t size,
  uint64_t &chunk_offset) const override;
};

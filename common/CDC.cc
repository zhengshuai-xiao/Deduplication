#include <random>

#include "CDC.h"
#include "fixedCDC.h"

std::unique_ptr<CDC> CDC::create(
  const std::string& type,
  int bits,
  int windowbits)
{
  if (type == "fixed") {
    return std::unique_ptr<CDC>(new FixedCDC(bits, windowbits));
  }
  return nullptr;
}


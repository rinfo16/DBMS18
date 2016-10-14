#include "common/bitmap.h"
#include <assert.h>
#include <memory.h>

namespace utils {

uint32_t msb32(uint32_t n)
{
  uint32_t b = 1;
  if (!n) return 0;

#define step(x) if (n >= ((uint32_t)1) << x) b <<= x, n >>= x
  step(16); step(8); step(4); step(2); step(1);
#undef step
  return b;
}

int msb32_idx(uint32_t n)
{
  int b = 0;
  if (!n) return -1;

#define step(x) if (n >= ((uint32_t)1) << x) b += x, n >>= x
  step(16); step(8); step(4); step(2); step(1);
#undef step
  return b;
}

#define lsb32(n) ( (uint32_t)(n) & -(int32_t)(n) )

/* finding the *position* of the least significant bit
   rarely makes sense, so we don't put much effort in it*/
inline int lsb32_idx(uint32_t n) { return msb32_idx(lsb32(n)); }



Bitmap::Bitmap(uint32_t length) {
  length_ = length;
  bitcount_ = length * 8;
  memset(bits_, 0, length);
}

Bitmap::~Bitmap() {
}

uint32_t Bitmap::FindFirstZeroBit() {

  uint32_t v = ~0;
  for (int i = 0; i * 8 < length_; i++) {
    uint32_t *bits = (uint32_t*) bits_ + i;
    if (*bits != v)
      return i * 32 + lsb32_idx(~*bits);
  }
  return bitcount_;
}

void Bitmap::SetBit(uint32_t offset, bool bit) {
  int bitmask = 0;
  uint8_t *bits = bits_ + offset / 8;
  bitmask = 1 << (offset % 8);
  if (bit)
    *bits |= bitmask;
  else
    *bits &= ~bitmask;
}

bool Bitmap::GetBit(uint32_t offset) {
  assert(offset < bitcount_);
  if (bits_[offset / 8] & (1 << (offset % 8)))
    return true;
  return false;
}

}  // namespace utils

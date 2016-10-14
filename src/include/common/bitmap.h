#ifndef COMMON_BITMAP_H_
#define COMMON_BITMAP_H_
#include "common/define.h"

namespace utils {

/*
 * Bitmap layout
 * 4 byte: bitmap length (total length of the memory block)
 * length_ - 8 byte: bits begin ...
 */

class Bitmap {
 public:
  Bitmap(uint32_t length);
  ~Bitmap();

  uint32_t Length() {
    return length_;
  }

  uint32_t BitCount() {
    return bitcount_;
  }

  uint32_t FindFirstZeroBit();

  void SetBit(uint32_t offset, bool bit);

  // return 0/1 (false/true)
  bool GetBit(uint32_t offset);

  // TODO .. bitmap AND/OR/XOR
  // Only used for 1BIT bitmap
  void And(const Bitmap *bitmap) {

  }

  void Or(const Bitmap *bitmap) {

  }

  void Xor(const Bitmap *bitmap) {

  }
 private:
  Bitmap();
  // DO NOT CHANGE THIS ORDER !!!
  uint32_t length_;
  uint32_t bitcount_;
  uint8_t bits_[0];  // offset bits begin ...
};

}  // namespace utils

#endif // COMMON_BITMAP_H_

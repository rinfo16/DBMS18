#ifndef COMMON_BITMAP_H_
#define COMMON_BITMAP_H_
#include "common/define.h"

namespace utils {

/*
 * Bitmap layout
 * 4 byte: bitmap length (total length of the memory block)
 * 4 byte: bitmap type (0/1)
 * length_ - 8 byte: bits begin ...
 */

enum BitmapType {
  kBitmap1Bit = 0,
  kBitmap2Bit = 1
};

class Bitmap {
 public:
  Bitmap();
  Bitmap(uint32_t length);
  ~Bitmap();

  // return 0 or 1
  uint8_t GetBit(uint32_t n) const {
    return 0;
  }

  // return 0, 1, 2 or 3
  uint8_t Get2Bit(uint32_t n) const {
    return 0;
  }

  // bit1 is 0 or 1
  // n: nth index of the bit (ONT bits as an unit)
  void SetBit(uint8_t bit1, uint32_t n) {
  }

  // bit2: is 0, 1, 2 or 3
  // n: nth index of the bits (TWO bits as an unit)
  void Set2Bit(uint8_t bit2, uint32_t n) {
  }

  // TODO .. bitmap AND/OR/XOR
  // Only used for 1BIT bitmap
  void And(const Bitmap *bitmap) {

  }

  void Or(const Bitmap *bitmap) {

  }

  void Xor(const Bitmap *bitmap) {

  }
 private:
  // DO NOT CHANGE THIS ORDER !!!
  uint32_t length_;
  uint32_t type_;
  uint8_t bits_[0];  // offset bits begin ...
};

}  // namespace utils

#endif // COMMON_BITMAP_H_

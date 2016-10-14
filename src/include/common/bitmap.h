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
  Bitmap();
  Bitmap(uint32_t length);
  ~Bitmap();

  uint32_t Length() {
    return length_;
  }

  uint32_t BitCount() {
    return bitcount_;
  }

  void Set(uint32_t start, uint32_t n = 1) {
  }

  void Clear(uint32_t start, uint32_t n = 1) {
  }

  uint32_t FindZero(uint32_t start, uint32_t n = 1) {
    return 0;
  }

  // return 0/1
  uint8_t GetBit(uint32_t nth_bit) {
    return 0;
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
  uint32_t bitcount_;
  uint8_t bits_[0];  // offset bits begin ...
};

}  // namespace utils

#endif // COMMON_BITMAP_H_

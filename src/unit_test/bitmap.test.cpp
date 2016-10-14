#include "common/bitmap.h"
#include <iostream>
#include "gtest/gtest.h"

#define BITMAP_SIZE 64
class BitmapTest : public testing::Test {
 protected:
  virtual void SetUp() {
    bitmap_ = new (bits_) utils::Bitmap(BITMAP_SIZE);
  }
  utils::Bitmap *bitmap_;
  uint8_t bits_[BITMAP_SIZE + sizeof(utils::Bitmap)];
};

TEST_F(BitmapTest, bitmap_get) {
  bitmap_->SetBit(0, true);
  bitmap_->SetBit(1, true);
  bitmap_->SetBit(3, true);
  bitmap_->SetBit(66, true);
  EXPECT_EQ(true, bitmap_->GetBit(0));
  EXPECT_EQ(true, bitmap_->GetBit(1));
  EXPECT_EQ(false, bitmap_->GetBit(2));
  EXPECT_EQ(true, bitmap_->GetBit(3));
  EXPECT_EQ(false, bitmap_->GetBit(30));

}

TEST_F(BitmapTest, bitmap_find0) {

  for (int i = 0; i < 89; i++) {
    bitmap_->SetBit(i, true);
  }
  EXPECT_EQ(89, bitmap_->FindFirstZeroBit());

  for (int i = 0; i < 100; i++) {
    bitmap_->SetBit(i, true);
  }
  EXPECT_EQ(100, bitmap_->FindFirstZeroBit());

  for (int i = 0; i < BITMAP_SIZE * 8; i++) {
    bitmap_->SetBit(i, true);
  }
  EXPECT_EQ(bitmap_->BitCount(), bitmap_->FindFirstZeroBit());
}

int main(int argc, char* argv[]) {
  std::cout << "Bitmap unit test !!!" << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

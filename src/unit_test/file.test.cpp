#include "storage/file.h"
#include "storage/storage_define.h"
#include <iostream>
#include "gtest/gtest.h"

class FileTest : public testing::Test {
 protected:
  virtual void SetUp() {
    std::string path;
    path = std::string() + std::string("/tmp/file.0");
    file1_ = new storage::File(path);
  }
  storage::File *file1_;
  storage::File *file2_;
  storage::File *file3_;
};

TEST_F(FileTest, file) {
  char buff1[32] = "test file data";
  char buff2[32];

  size_t off = 100000;
  EXPECT_EQ(true, file1_->Create());
  EXPECT_EQ(true, file1_->Open());
  EXPECT_EQ(true, file1_->Write(off, buff1, sizeof(buff1)));
  EXPECT_EQ(true, file1_->Read(off, buff2, sizeof(buff2)));
}

int main(int argc, char* argv[]) {
  std::cout << "File unit test !!!" << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

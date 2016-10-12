#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "file.h"

namespace storage {

File::~File() {
  Close();
}

File::File(const std::string path)
    : path_(path),
      fd_(-1) {
}

bool File::Create() {
  fd_ = ::creat(path_.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd_ < 0) {
    perror("open");
    std::cerr << "file [" << path_ << "] open failed." << std::endl;
    return false;
  }
  Close();
  return true;
}

bool File::Open() {
  fd_ = ::open(path_.c_str(), O_RDWR);
  if (fd_ < 0) {
    perror("open");
    std::cerr << "file [" << path_ << "] open failed." << std::endl;
    return false;
  }
  return true;
}

bool File::Read(size_t offset, void *buff, int length) {
  int ret = ::lseek(fd_, offset, SEEK_SET);
  if (ret < 0) {
    perror("lseek");
    std::cerr << "file [" << path_ << "] seek offset [" << offset << "] failed."
              << std::endl;
    return false;
  }

  ret = ::read(fd_, buff, length);
  if (ret < 0) {
    perror("read");
    std::cerr << "file [" << path_ << "] read offset [" << offset << "] failed."
              << std::endl;
    return false;
  }
  return true;
}

bool File::Write(size_t offset, const void *buff, int length) {
  int ret = ::lseek(fd_, offset, SEEK_SET);
  if (ret < 0) {
    perror("lseek");
    std::cerr << "file [" << path_ << "] seek offset [" << offset << "] failed."
              << std::endl;
    return false;
  }

  ret = ::write(fd_, buff, length);
  if (ret < 0) {
    perror("read");
    std::cerr << "file [" << path_ << "] write offset [" << offset
              << "] failed." << std::endl;
    return false;
  }
  return true;
}

void File::Close() {
  ::close(fd_);
  fd_ = -1;
}

}  // namespace storage

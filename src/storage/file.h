#ifndef SRC_STORAGE_FILE_H_
#define SRC_STORAGE_FILE_H_

#include <stdlib.h>
#include <string>

namespace storage {

class File {
 public:
  File(const std::string path);
  virtual ~File();

  // Create a file, but do not open it
  bool Create();

  bool Open();
  void Close();
  bool Read(size_t offset, void *buff, int length);
  bool Write(size_t offset, const void *buff, int length);

 private:
  std::string path_;
  int fd_;
};

}  // namespace storage

#endif // SRC_STORAGE_FILE_H_

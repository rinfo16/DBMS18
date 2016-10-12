#ifndef STORAGE_DEFINE_H_
#define STORAGE_DEFINE_H_

#include "common/define.h"

const char *kDataPath = "/tmp";

typedef size_t frame_index_t;


struct PageID {
  fileno_t fileno_;
  pageno_t blockno_;

  bool operator <(const PageID id) const {
    if (fileno_ == id.fileno_) {
      return blockno_ < id.blockno_;
    }

    return fileno_ < id.fileno_;
  }
};

#endif // STORAGE_DEFINE_H_

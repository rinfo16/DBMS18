#ifndef STORAGE_DEFINE_H_
#define STORAGE_DEFINE_H_

#include "common/define.h"



typedef size_t frame_index_t;

struct PageID {
  fileno_t fileno_;
  pageno_t blockno_;

  bool Invalid() {
    return fileno_ == 0 && blockno_ == 0;
  }

  PageID() {
    fileno_ = 0;
    blockno_ = 0;
  }
  bool operator <(const PageID id) const {
    if (fileno_ == id.fileno_) {
      return blockno_ < id.blockno_;
    }

    return fileno_ < id.fileno_;
  }
};

struct SegmentDescripter {
  segmentno_t segmentno_;
  PageID first_page_id_;
};

#endif // STORAGE_DEFINE_H_

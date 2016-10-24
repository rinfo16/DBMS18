#ifndef PAGE_ID_H_
#define PAGE_ID_H_

struct PageID {
  fileno_t fileno_;
  pageno_t blockno_;

  bool Invalid() {
    return fileno_ == ~0 && blockno_ == ~0;
  }

  PageID() {
    fileno_ = ~0;
    blockno_ = ~0;
  }
  bool operator <(const PageID id) const {
    if (fileno_ == id.fileno_) {
      return blockno_ < id.blockno_;
    }

    return fileno_ < id.fileno_;
  }

  bool operator == (const PageID id) const {
    return fileno_ == id.fileno_ && blockno_ == id.blockno_;
  }
};

#endif // PAGE_ID_H_

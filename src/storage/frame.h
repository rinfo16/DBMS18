#ifndef FRAME_H_
#define FRAME_H_

#include "common/define.h"

namespace storage {

class Frame {
 public:
  Frame()
      : frame_index_(0),
        fix_count_(0),
        min_lsn_(0),
        flag_(0) {
  }

  Frame(uint32_t frame_index)
      : frame_index_(frame_index),
        fix_count_(0),
        min_lsn_(0),
        flag_(0) {
  }

  void FixPage() {
    fix_count_++;
  }

  void UnfixPage() {
    fix_count_--;
  }

  void SetFree(bool is_free) { flag_ = (int32_t)is_free; }

  bool IsFree() { return flag_ == (int32_t)true ; }

  void *GetPage() { return page_data_;}

 private:
  uint32_t frame_index_;
  uint32_t fix_count_;
  uint32_t min_lsn_;
  uint32_t flag_;
  PageID page_id_;
  uint8_t page_data_[0];
};

}  // end namespace storage

#endif // FRAME_H_

#ifndef FRAME_H_
#define FRAME_H_

#include "common/define.h"
#include "page.h"
namespace storage {

class Frame {
 public:
  Frame()
      : frame_index_(0),
        fix_count_(0),
        free_(0),
        dirty_(0) {
  }

  Frame(uint32_t frame_index)
      : frame_index_(frame_index),
        fix_count_(0),
        free_(0),
        dirty_(0) {
  }

  void FixPage();

  void UnfixPage();

  void SetFree(bool is_free);

  bool IsFree() const;

  void SetDirty(bool dirty);

  bool IsDirty() const;

  Page *GetPage();

 private:
  uint32_t frame_index_;
  uint32_t fix_count_;
  uint8_t free_;
  uint8_t dirty_;
};

}  // end namespace storage

#endif // FRAME_H_

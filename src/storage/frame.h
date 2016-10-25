#ifndef FRAME_H_
#define FRAME_H_

#include "../include/common/list.hpp"
#include "common/define.h"
#include "page_layout.h"

namespace storage {

class PageFrame {
 public:
  PageFrame():
        fix_count_(0),
        dirty_(0),
        page_(NULL),
        is_hir_(true) {
  }

  void FixPage();

  void UnfixPage();

  void SetDirty(bool dirty);

  bool IsDirty() const;

  uint32_t FixCount() const;

  void SetHIR() {
    is_hir_ = true;
  }

  void SetLIR() {
    is_hir_ = false;
  }

  bool IsHIR() const {
    return is_hir_;
  }

  bool IsLIR() const {
    return !is_hir_;
  }

  PageID GetPageID() {
    return pageid_;
  }

  void SetPageID(PageID id) {
    pageid_ = id;
  }

  Page *GetPage() {
    return page_;
  }

  void SetPage(Page*page);

 private:

  uint32_t fix_count_;
  uint8_t dirty_;
  bool is_hir_;  // ture (HIR) false (LIR)
  PageID pageid_;
  Page* page_;
};

typedef utils::ListNode<PageFrame> Frame;
typedef utils::List<PageFrame> FrameList;

Frame *PageGetFrame(Page *page);
void PageSetFrame(Page *page, Frame *frame);

}  // end namespace storage

#endif // FRAME_H_

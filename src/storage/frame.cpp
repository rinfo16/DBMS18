#include "frame.h"

namespace storage {

void PageFrame::FixPage() {
  fix_count_++;
}

void PageFrame::UnfixPage() {
  fix_count_--;
}

void PageFrame::SetDirty(bool dirty) {
  dirty_ = (int8_t) dirty;
}

bool PageFrame::IsDirty() const {
  return dirty_ == (int8_t) true;
}

uint32_t PageFrame::FixCount() const {
  return fix_count_;
}

void PageFrame::SetPage(Page *page) {
  if (page != NULL)
    pageid_ = page->pageid_;
  page_ = page;
}

Frame *PageGetFrame(Page *page) {
  return (Frame*) page->frame_ptr_;
}

void PageSetFrame(Page *page, Frame *frame) {
  page->frame_ptr_ = (uint64_t) frame;
}

}  // end namespace storage

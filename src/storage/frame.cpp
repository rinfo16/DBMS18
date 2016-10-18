#include "frame.h"

namespace storage {

void PageFrame::FixPage() {
  fix_count_++;
}

void PageFrame::UnfixPage() {
  fix_count_--;
}

void PageFrame::SetFree(bool is_free) {
  free_ = (int8_t) is_free;
}

bool PageFrame::IsFree() const {
  return free_ == (int8_t) true;
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

uint32_t PageFrame::GetFrameIndex() {
  return frame_index_;
}

void PageFrame::SetFrameIndex(uint32_t frame_index) {
  frame_index_ = frame_index;
}

void PageFrame::SetPage(Page *page) {
  if (page != NULL)
    pageid_ = page->pageid_;
  page_ = page;
}

Page *FrameToPage(Frame *frame) {
  Page* page = (Page*) (((uint8_t*) frame) + sizeof(Frame));
  return page;
}

Frame *PageGetFrame(Page *page) {
  return (Frame*) page->frame_ptr_;
}

void PageSetFrame(Page *page, Frame *frame) {
  page->frame_ptr_ = (uint64_t) frame;
}

}  // end namespace storage

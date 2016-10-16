#include "frame.h"

namespace storage {

void Frame::FixPage() {
  fix_count_++;
}

void Frame::UnfixPage() {
  fix_count_--;
}

void Frame::SetFree(bool is_free) {
  free_ = (int8_t) is_free;
}

bool Frame::IsFree() const {
  return free_ == (int8_t) true;
}

void Frame::SetDirty(bool dirty) {
  dirty_ = (int8_t) dirty;
}

bool Frame::IsDirty() const {
  return dirty_ == (int8_t) true;
}

uint32_t Frame::FixCount() const {
  return fix_count_;
}

Page *Frame::GetPage() {
  return (Page*) (((uint8_t*) this) + sizeof(*this));
}

uint32_t Frame::GetFrameIndex() {
  return frame_index_;
}

void Frame::SetFrameIndex(uint32_t frame_index) {
  frame_index_ = frame_index;
}

Frame* Frame::ToFrame(Page *page) {
  Frame *frame = (Frame*) ((uint8_t *) page - sizeof(Frame));
  return frame;
}

}  // end namespace storage

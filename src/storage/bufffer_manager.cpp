#include "bufffer_manager.h"

namespace storage {

BuffferManager::BuffferManager(size_t pool_size) {
  buffer_pool_ = new Frame[pool_size];
  pool_size_ = pool_size;
}

BuffferManager::~BuffferManager() {
  delete[] buffer_pool_;
}

bool BuffferManager::FixPage(PageID id, PageControl* param) {
  frame_index_t frame_index = 0;
  auto iter = loaded_frames_.find(id);
  if (iter == loaded_frames_.end()) {  // cannot find page, read from disk
    frame_index = LocatePage(id, param->is_new_);
  } else {
    frame_index = iter->second;
  }
  if (frame_index == 0) {
    return false;
  }
  Frame* frame = &(buffer_pool_[frame_index]);
  frame->FixPage();

  param->page_ = (Page*) frame->GetPage();
  // TODO .. do something
  return true;
}

bool BuffferManager::UnfixPage(PageID id) {
  auto iter = loaded_frames_.find(id);
  if (iter != loaded_frames_.end()) {  // cannot find page, read from disk
    frame_index_t frame_index_ = GetFrame();
    Frame *frame  = &(buffer_pool_[frame_index_]);
    frame->UnfixPage();
  } else {
    return false;
  }
  return true;
}

frame_index_t BuffferManager::LocatePage(PageID id, bool is_new) {
  frame_index_t frame_index = GetFrame();
  if (frame_index == 0) {  // TODO  invalid frame index
    return frame_index;
  }

  Frame *frame = & (buffer_pool_[frame_index]);

  if (!is_new) {  // read from disk
    // TODO
  } else {  // a new page
    // TODO
  }

  loaded_frames_.insert(std::make_pair(id, frame_index));

  return frame_index;
}

frame_index_t BuffferManager::GetFrame() {
  frame_index_t frame_index = 0;
  if (free_frames_.empty()) {
    // TODO find unfixed page in loaded_frames_, if there is not return false
    // LRU
    // 1. It's better to find a clean page
    // 2. If there is not a clean page, flush the dirty page to disk
  } else {
    frame_index_t frame_index = free_frames_.back();
    free_frames_.pop_back();
  }
  return frame_index;
}
}  // namespace storage

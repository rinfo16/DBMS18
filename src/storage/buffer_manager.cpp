#include "buffer_manager.h"

namespace storage {

BufferManager::BufferManager(size_t pool_size, size_t page_size) {
  buffer_pool_ = new Frame[pool_size];
  pool_size_ = pool_size;
  page_size_ = page_size;
}

BufferManager::~BufferManager() {
  delete[] buffer_pool_;
}

Page* BufferManager::FixPage(PageID id, bool is_new) {
  frame_index_t frame_index = 0;
  LoadedFrameMap::iterator iter = loaded_frames_.find(id);
  if (iter == loaded_frames_.end()) {  // cannot find page, read from disk
    frame_index = LocatePage(id, is_new);
  } else {
    frame_index = iter->second;
  }
  if (frame_index == 0) {
    return NULL;
  }
  Frame* frame = FrameAt(frame_index);
  frame->FixPage();

  // TODO .. do something
  return (Page*) frame->GetPage();

}

bool BufferManager::UnfixPage(PageID id) {
  LoadedFrameMap::iterator iter = loaded_frames_.find(id);
  if (iter != loaded_frames_.end()) {  // cannot find page, read from disk
    frame_index_t frame_index = GetFrame();
    Frame *frame = FrameAt(frame_index);
    frame->UnfixPage();
  } else {
    return false;
  }
  return true;
}

frame_index_t BufferManager::LocatePage(PageID id, bool is_new) {
  frame_index_t frame_index = GetFrame();
  if (frame_index == 0) {  // TODO  invalid frame index
    return frame_index;
  }

  Frame *frame = FrameAt(frame_index);

  if (!is_new) {  // read from disk
    // TODO
  } else {  // a new page
    // TODO
  }

  loaded_frames_.insert(std::make_pair(id, frame_index));

  return frame_index;
}

frame_index_t BufferManager::GetFrame() {
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

void BufferManager::FlushPage(frame_index_t frame_index) {
  Frame *frame = FrameAt(frame_index);
  assert(!frame->IsFree() && frame->IsDirty());
  Page *page = frame->GetPage();
  PageID page_id = page->pageid_;
  FileMap::iterator iter = files_.find(page_id.fileno_);
  assert(iter != files_.end());
  File *file = iter->second;
  bool ok = file->Write(page_id.blockno_ * page_size_, page, page_size_);
  assert(ok);
  frame->SetDirty(false);
}

Frame *BufferManager::FrameAt(frame_index_t frame_index) {
  return &(buffer_pool_[frame_index]);
}

}
// namespace storage

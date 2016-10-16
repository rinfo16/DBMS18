#include "buffer_manager.h"
#include <sstream>
#include <iostream>

namespace storage {

BufferManager::BufferManager(size_t pool_size, size_t page_size,
                             std::string data_path) {
  buffer_pool_ = NULL;
  pool_size_ = pool_size;
  page_size_ = page_size;
  data_path_ = data_path;
  frame_count_ = 0;
  frame_size_ = 0;
}

BufferManager::~BufferManager() {
  Stop();
}

bool BufferManager::Start() {
  frame_size_ = page_size_ + sizeof(Frame);
  buffer_pool_ = new uint8_t[pool_size_];
  frame_count_ = pool_size_ / frame_size_;
  free_frames_.reserve(frame_count_);
  for (uint32_t i = 0; i < frame_count_; i++) {
    free_frames_.push_back(i);
    Frame *frame = new (buffer_pool_ + i * frame_size_) Frame(i);
  }
  return true;
}

void BufferManager::Stop()
{
  if (buffer_pool_ == NULL)
    return;

  FlushAll();
  for (FileMap::iterator iter = files_.begin(); iter != files_.end(); iter++)
  {
    delete iter->second;
  }
  files_.clear();
  delete [] buffer_pool_;
  buffer_pool_ = NULL;
  free_frames_.clear();
  loaded_frames_.clear();
}

Page* BufferManager::FixPage(PageID id, bool is_new) {
  LoadedFrameMap::iterator iter = loaded_frames_.find(id);
  Frame* frame = NULL;
  if (iter == loaded_frames_.end()) {  // cannot find page, read from disk
    frame = LocatePage(id, is_new);
  } else {
    frame_index_t frame_index = iter->second;
    frame = FrameAt(frame_index);
  }
  if (frame == 0) {
    assert(false);
    return NULL;
  }

  frame->FixPage();

  // TODO .. do something
  return (Page*) frame->GetPage();

}

bool BufferManager::UnfixPage(PageID id) {
  LoadedFrameMap::iterator iter = loaded_frames_.find(id);
  if (iter != loaded_frames_.end()) {  // cannot find page, read from disk
    frame_index_t frame_index = iter->second;
    Frame *frame = FrameAt(frame_index);
    frame->UnfixPage();
  } else {
    assert(false);
    return false;
  }
  return true;
}

Frame* BufferManager::LocatePage(PageID id, bool is_new) {
  Frame *frame = GetFrame();
  if (frame == NULL)
  {
    assert(false);
    return NULL;
  }
  if (!is_new) {  // read from disk
    ReadPage(id, frame->GetPage());
  } else {  // a new page

  }

  loaded_frames_.insert(std::make_pair(id, frame->GetFrameIndex()));

  return frame;
}

Frame* BufferManager::GetFrame() {
  Frame *frame = NULL;
  if (free_frames_.empty()) {
    // TODO find unfixed page in loaded_frames_, if there is not return false
    // LRU
    // 1. It's better to find a clean page
    // 2. If there is not a clean page, flush the dirty page to disk
  } else {
    frame_index_t frame_index = free_frames_.back();
    frame = FrameAt(frame_index);
    free_frames_.pop_back();
  }
  return frame;
}

Frame *BufferManager::FrameAt(frame_index_t frame_index) {
  return (Frame*)(buffer_pool_ + frame_size_ * frame_index);
}

void BufferManager::FlushAll() {
  for (int i = (int)(frame_count_-1); i >= 0 ; i--) {
    if (FrameAt(i)->IsDirty()) {
      FlushPage(i);
    }
  }
}

void BufferManager::FlushPage(frame_index_t i) {
  WritePage((Page*) FrameAt(i)->GetPage());
}

void BufferManager::ReadPage(PageID pageid, Page *page) {
  File *f = GetFile(pageid.fileno_);
  if (f != NULL) {
    f->Read(pageid.blockno_ * page_size_, page, page_size_);
  }
}

void BufferManager::WritePage(Page *page) {
  File *f = GetFile(page->pageid_.fileno_);
  if (f != NULL) {
    f->Write(page->pageid_.blockno_ * page_size_, page, page_size_);
    Frame::ToFrame(page)->SetDirty(false);
  }
}

File* BufferManager::GetFile(fileno_t no) {
  FileMap::iterator iter = files_.find(no);
  File *f = NULL;
  if (iter == files_.end()) {
    std::stringstream ssm;
    ssm << data_path_ << "/nutshell.data." << no;
    f = new File(ssm.str());
    if (!f->Open()) {
      return NULL;
    }
    files_.insert(std::make_pair(no, f));

  } else {
    f = iter->second;
  }
  return f;
}

}
// namespace storage

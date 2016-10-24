#include "buffer_manager.h"
#include <memory.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "common/config.h"

#define LIRS 0
namespace storage {

BufferManager::BufferManager() {
  buffer_pool_ = NULL;
  pool_size_ = config::Setting::instance().buffer_pool_size_;
  page_size_ = config::Setting::instance().page_size_;
  data_path_ = config::Setting::instance().data_directory_;
  page_count_ = pool_size_ / page_size_;
  pool_size_ = page_count_ * page_size_;
  frame_count_ = 0;
  frame_size_ = 0;
}

BufferManager::~BufferManager() {
  Stop();
}

bool BufferManager::Start() {
  buffer_pool_ = new uint8_t[pool_size_];
  memset(buffer_pool_, ~0, pool_size_);
  free_buffer_index_.reserve(page_count_);
  for (uint32_t i = 0; i < page_count_; i++) {
    free_buffer_index_.push_back(i);
  }
  return true;
}

void BufferManager::Stop() {
  if (buffer_pool_ == NULL)
    return;

  FlushAll();

  for (FileMap::iterator iter = files_.begin(); iter != files_.end(); iter++) {
    delete iter->second;
  }
  files_.clear();
  delete[] buffer_pool_;
  buffer_pool_ = NULL;
  free_buffer_index_.clear();
  loaded_buffers_.clear();
  stack_s_.clear();
  stack_q_.clear();
}

Page* BufferManager::FixPage(PageID id, bool is_new) {

  LoadedBufferMap::iterator frame_iter = loaded_buffers_.find(id);
  Page *page = NULL;
  Frame* frame = NULL;
  bool non_resident = false;
  bool find_in_buffers = false;
#if LIRS
  if (frame_iter == loaded_buffers_.end()) {
    // cannot find a the frame, get a free page, or VICTIM one;
    non_resident = true;
    page = GetFreePage();
    assert(page);
    frame = new Frame();  // TODO delete this ???
    frame->SetPage(page);
    frame->SetHIR();
    loaded_buffers_.insert(std::make_pair(id, frame));
  } else {
    frame = frame_iter->second;
    assert(frame);
    page = frame->GetPage();
    if (frame->IsLIR()) {
      assert(FrameToPage(frame));
      RemoveFromStackS(frame);
    } else {
      assert(frame->IsHIR());
      if (page != NULL) {
        bool in_stack_s = RemoveFromStackS(frame);
        if (in_stack_s) {
          frame->SetLIR();
          RemoveFromStackQ(frame);
          SHeadToQTail();
        } else {
          RemoveFromStackQ(frame);
        }
      } else {
        page = GetFreePage();
        assert(page);
        frame->SetPage(page);
        frame->SetLIR();
        page->frame_ptr_ = (uint64_t) page;
        non_resident = true;
        RemoveFromStackS(frame);
      }
    }
  }
  if (non_resident && !is_new) {
    ReadPage(id, page);
  }

  if (frame == 0) {
    assert(false);
    return NULL;
  }
#else
  if (frame_iter == loaded_buffers_.end()) {
    page = LocatePage(id, is_new);
    if (page == NULL) {
      return NULL;
    }
    frame = new Frame();
    frame->SetPage(page);
    loaded_buffers_.insert(std::make_pair(id, frame));
  } else {
    frame = frame_iter->second;
    page = frame->GetPage();
  }
#endif
  PageSetFrame(page, frame);
  frame->FixPage();

  return frame->GetPage();

}

bool BufferManager::UnfixPage(PageID id) {
#if LIRS
  LoadedBufferMap::iterator iter = loaded_buffers_.find(id);
  if (iter != loaded_buffers_.end()) {
    Frame *frame = iter->second;
    frame->UnfixPage();

    if (frame->FixCount() == 0) {
      stack_q_.erase(std::remove(stack_q_.begin(), stack_q_.end(), frame),
                     stack_q_.end());
      stack_s_.erase(std::remove(stack_s_.begin(), stack_s_.end(), frame),
                     stack_s_.end());
      if (frame->IsLIR()) {
        assert(frame->GetPage() && frame->IsLIR());
        stack_s_.push_back(frame);
      } else {
        stack_s_.push_back(frame);
        stack_q_.push_back(frame);
        assert(frame->GetPage() && frame->IsHIR());
      }
    }
  } else {
    assert(false);
    return false;
  }
  return true;
#else
  LoadedBufferMap::iterator iter = loaded_buffers_.find(id);
  if (iter != loaded_buffers_.end()) {
    Frame *frame = iter->second;
    frame->UnfixPage();
  } else {
    assert(false);
    return false;
  }
  return true;
#endif

}

Page* BufferManager::LocatePage(PageID id, bool is_new) {
  Page *page = GetFreePage();
  if (page == NULL) {
    assert(false);
    return NULL;
  }
  if (!is_new) {  // read from disk
    ReadPage(id, page);
  }

  return page;
}

Page* BufferManager::GetFreePage() {
  Page *page = NULL;
  if (free_buffer_index_.empty()) {
    page = Victim();
  } else {
    buffer_index_t buffer_index = free_buffer_index_.back();
    page = BufferAt(buffer_index);
    free_buffer_index_.pop_back();
  }
  if (page != NULL) {
    memset(page, 0, page_size_);
  }
  return page;
}

Page *BufferManager::BufferAt(buffer_index_t frame_index) {
  return (Page*) (buffer_pool_ + page_size_ * frame_index);
}

void BufferManager::FlushAll() {
  for (auto iter = loaded_buffers_.begin(); iter != loaded_buffers_.end();
      iter++) {
    Frame *frame = iter->second;
    if (frame->GetPage() && frame->IsDirty()) {
      if (frame->GetPage()->pageid_.blockno_ == 0
          && frame->GetPage()->pageid_.fileno_ == 0) {
        int i = 0;
      }
      WritePage(frame->GetPage());
    }
  }
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
    Frame *frame = PageGetFrame(page);
    PageSetFrame(page, NULL);
    f->Write(page->pageid_.blockno_ * page_size_, page, page_size_);
    frame->SetDirty(false);
    PageSetFrame(page, frame);
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

bool BufferManager::RemoveFromStackS(Frame *frame) {
  FrameStack::iterator iter = std::find(stack_s_.begin(), stack_s_.end(),
                                        frame);
  bool ret = false;
  if (iter != stack_s_.end()) {
    if (iter == stack_s_.begin()) {
      stack_s_.erase(iter);
      RemoveUnResidentHIRPage();
    } else {
      stack_s_.erase(iter);
    }
    return true;
  }
  return false;
}

bool BufferManager::RemoveFromStackQ(Frame *frame) {
  FrameStack::iterator iter = std::find(stack_q_.begin(), stack_q_.end(),
                                        frame);
  bool ret = false;
  if (iter != stack_q_.end()) {
    stack_q_.erase(iter);
    ret = true;
  } else {
    ret = false;
  }
  return ret;
}

void BufferManager::SHeadToQTail() {
  FrameStack::iterator iter = stack_s_.begin();
  if (iter != stack_s_.end()) {
    Frame *frame = *iter;
    frame->SetHIR();
    assert(frame->GetPage());
    stack_s_.erase(iter);
    stack_q_.erase(std::remove(stack_q_.begin(), stack_q_.end(), frame),
                   stack_q_.end());
    stack_q_.push_back(frame);
    RemoveUnResidentHIRPage();
  }
}

void BufferManager::RemoveUnResidentHIRPage() {
  FrameStack::iterator iter;
  for (iter = stack_s_.begin(); iter != stack_s_.end(); iter++) {
    Frame *f = *iter;
    if (FrameToPage(f) == NULL) {
      size_t ret = loaded_buffers_.erase(f->GetPageID());
      assert(ret == 1);
      delete f;
      iter = stack_s_.erase(iter);
    } else {
      break;
    }
  }
}

Page* BufferManager::Victim() {
  Page *page = NULL;
#if LIRS
  if (stack_q_.empty()) {
    assert(false);
    return NULL;
  }
  Frame *frame = stack_q_.front();
  if (frame->FixCount() > 0) {
    // Logic error, should not be here
    return NULL;
  }
  page = frame->GetPage();
  if (frame->IsDirty()) {
    WritePage(page);
  }
  frame->SetPage(NULL);
  stack_q_.pop_front();
  FrameStack::iterator iter = std::find(stack_s_.begin(), stack_s_.end(),
                                        frame);
  if (iter == stack_s_.end()) {
    size_t ret = loaded_buffers_.erase(frame->GetPageID());
    assert(ret == 1);
    delete frame;
  }
#else
  for (LoadedBufferMap::iterator iter = loaded_buffers_.begin();
      iter != loaded_buffers_.end(); iter++) {
    Frame *frame = (iter->second);
    if (frame->FixCount() == 0) {
      if (frame->IsDirty()) {
        WritePage(frame->GetPage());
        frame->SetDirty(false);
      }
      page = frame->GetPage();
      loaded_buffers_.erase(iter);
      delete frame;
      break;
    }
  }
#endif
  return page;
}

}
// namespace storage

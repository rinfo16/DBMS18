#ifndef BUFFFER_MANAGER_H_
#define BUFFFER_MANAGER_H_

#include <map>
#include <vector>
#include <unordered_map>
#include <deque>
#include <assert.h>
#include "common/hash.h"
#include "storage_define.h"
#include "frame.h"
#include "file.h"
#include "page_layout.h"

namespace storage {

struct HashFunction {
  size_t operator ()(const PageID &id) const {
    return (size_t) (*(size_t*) (&id));
  }
};

struct EqualTo {
  bool operator()(const PageID & id1, const PageID & id2) const {
    return id1.blockno_ == id2.blockno_ && id1.fileno_ == id2.fileno_;
  }
};

typedef std::unordered_map<PageID, Frame*, HashFunction, EqualTo> LoadedBufferMap;
typedef std::map<fileno_t, File *> FileMap;
//typedef std::map<PageID, frame_index_t> LoadedFrameMap;
typedef std::vector<size_t> FreeBufferIndexList;

typedef std::deque<Frame*> FrameStack;

class BufferManager {
 public:
  BufferManager();

  virtual ~BufferManager();

  bool Start();

  void Stop();

  Page* FixPage(PageID id, bool is_new);

  bool UnfixPage(PageID id);

  void FlushAll();

 private:

  Page* LocatePage(PageID id, bool is_new);

  Page* GetFreePage();

  void ReadPage(PageID pageid, Page *page);

  void WritePage(Page *page);

  File* GetFile(fileno_t no);

  Page *BufferAt(buffer_index_t frame_index);

  Page *Victim();

  bool RemoveFromStackS(Frame *frame);

  bool RemoveFromStackQ(Frame *frame);

  void SHeadToQTail();

  void RemoveUnResidentHIRPage();

  uint8_t *buffer_pool_;
  uint32_t pool_size_;
  uint32_t page_size_;
  uint32_t frame_count_;
  uint32_t frame_size_;
  uint32_t page_count_;
  std::string data_path_;

  FreeBufferIndexList free_buffer_index_;
  LoadedBufferMap loaded_buffers_;

  // stack S and Q used by LIRS cache algorithms
  // TODO .. this is inefficient
  FrameStack stack_s_;
  FrameStack stack_q_;

  FileMap files_;
};

}  // namespace storage

#endif // BUFFFER_MANAGER_H_

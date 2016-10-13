#ifndef BUFFFER_MANAGER_H_
#define BUFFFER_MANAGER_H_

#include <map>
#include <vector>
#include <assert.h>
#include "storage_define.h"
#include "frame.h"
#include "file.h"
#include "page_layout.h"

namespace storage {

struct PageControl {
  bool is_new_;
  Page *page_;
};

typedef std::map<fileno_t, File *> FileMap;
typedef std::map<PageID, frame_index_t> LoadedFrameMap;
typedef std::vector<size_t> FreeFrameList;

class BufferManager {
 public:
  BufferManager(size_t pool_size, size_t page_size);
  virtual ~BufferManager();

  bool FixPage(PageID id, PageControl * param);

  bool UnfixPage(PageID id);

  frame_index_t LocatePage(PageID id, bool is_new);

  frame_index_t GetFrame();

 private:
  void FlushPage(frame_index_t frame_index);

  void FlushAll();

  Frame *FrameAt(frame_index_t frame_index);

  Frame *buffer_pool_;
  size_t pool_size_;
  size_t page_size_;
  FreeFrameList free_frames_;  // TODO use hash table ...
  LoadedFrameMap loaded_frames_;
  FileMap files_;
};

}  // namespace storage

#endif // BUFFFER_MANAGER_H_

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

typedef std::map<fileno_t, File *> FileMap;
typedef std::map<PageID, frame_index_t> LoadedFrameMap;
typedef std::vector<size_t> FreeFrameList;

class BufferManager {
 public:
  BufferManager(size_t pool_size, size_t page_size, std::string path);
  virtual ~BufferManager();

  bool Start();

  Page* FixPage(PageID id, bool is_new);

  bool UnfixPage(PageID id);

  frame_index_t LocatePage(PageID id, bool is_new);

  frame_index_t GetFrame();

  void FlushAll();
  void FlushPage(frame_index_t frame_index);

 private:
  void ReadPage(PageID pageid, Page *page);

  void WritePage(Page *page);
  File* GetFile(fileno_t no);
  Frame *FrameAt(frame_index_t frame_index);

  Frame *buffer_pool_;
  size_t pool_size_;
  size_t page_size_;
  std::string data_path_;
  FreeFrameList free_frames_;  // TODO use hash table ...
  LoadedFrameMap loaded_frames_;
  FileMap files_;
};

}  // namespace storage

#endif // BUFFFER_MANAGER_H_

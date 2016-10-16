#ifndef BUFFFER_MANAGER_H_
#define BUFFFER_MANAGER_H_

#include <map>
#include <vector>
#include <unordered_map>
#include <assert.h>
#include "common/hash.h"
#include "storage_define.h"
#include "frame.h"
#include "file.h"
#include "page_layout.h"

namespace storage {

struct HashFunction
{
  size_t operator () (const PageID &id) const{
    return (size_t)utils::Hash64(&id, sizeof(id));
  }
};

struct EqualTo
{
  bool operator()(const PageID & id1, const PageID & id2) const{
    return id1.blockno_ == id2.blockno_ && id1.fileno_ == id2.fileno_;
  }
};

typedef std::unordered_map <PageID, frame_index_t, HashFunction, EqualTo> LoadedFrameMap;
typedef std::map<fileno_t, File *> FileMap;
//typedef std::map<PageID, frame_index_t> LoadedFrameMap;
typedef std::vector<size_t> FreeFrameList;

class BufferManager {
 public:
  BufferManager(size_t pool_size, size_t page_size, std::string path);
  virtual ~BufferManager();

  bool Start();

  void Stop();

  Page* FixPage(PageID id, bool is_new);

  bool UnfixPage(PageID id);

  Frame* LocatePage(PageID id, bool is_new);

  Frame* GetFrame();

  void FlushAll();
  void FlushPage(frame_index_t frame_index);

 private:
  void ReadPage(PageID pageid, Page *page);

  void WritePage(Page *page);
  File* GetFile(fileno_t no);
  Frame *FrameAt(frame_index_t frame_index);

  uint8_t *buffer_pool_;
  uint32_t pool_size_;
  uint32_t page_size_;
  uint32_t frame_count_;
  uint32_t frame_size_;
  std::string data_path_;
  FreeFrameList free_frames_;
  LoadedFrameMap loaded_frames_;
  FileMap files_;
};

}  // namespace storage

#endif // BUFFFER_MANAGER_H_

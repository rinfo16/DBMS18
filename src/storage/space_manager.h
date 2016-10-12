#ifndef SRC_STORAGE_SPACE_MANAGER_H_
#define SRC_STORAGE_SPACE_MANAGER_H_

#include "buffer_manager.h"
#include "storage_define.h"

namespace storage {

class SpaceManager {
 public:
  SpaceManager();
  virtual ~SpaceManager();

  bool InitDB();

  bool CreateFile(fileno_t fileno);

  bool CreateSegment(PageID *segment_header_page);

  bool CreateExtent(PageID segment_header_page, PageID *extent_header_page);

  bool FindSpace(PageID segment_header_page, size_t length, PageID *data_page_);

 private:
  std::vector<pageno_t> data_file_;
  pageno_t next_page_no_;
  BufferManager *buffer_manager_;
};

}  // namespace storage

#endif // SRC_STORAGE_SPACE_MANAGER_H_

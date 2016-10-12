#ifndef SRC_STORAGE_SPACE_MANAGER_H_
#define SRC_STORAGE_SPACE_MANAGER_H_

#include "buffer_manager.h"
#include "storage_define.h"

namespace storage {

class SpaceManager {
 public:
  SpaceManager();
  virtual ~SpaceManager();

  bool CreateSegment(PageID *segment_header_page);

  bool CreateExtent(PageID segment_header_page, PageID *extent_header_page);

  bool FindSpace(PageID segment_header_page, size_t length, PageID *data_page_);
};

}  // namespace storage

#endif // SRC_STORAGE_SPACE_MANAGER_H_

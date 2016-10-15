#ifndef SRC_STORAGE_SPACE_MANAGER_H_
#define SRC_STORAGE_SPACE_MANAGER_H_

#include "buffer_manager.h"
#include "storage_define.h"

namespace storage {

class SpaceManager {
 public:
  SpaceManager(BufferManager *buffer_manager);

  virtual ~SpaceManager();

  bool InitDB();

  bool CreateFile(fileno_t fileno);

  bool CreateDataFile(fileno_t *fileno);

  bool CreateSegment(PageID *segment_header_page);

  bool CreateExtentInSegment(
                  Page* segment_header_page,
                    PageID *extent_header_page_id);

  bool WriteTuple(PageID segment_header_pageid,
                  void *tuple,
                 uint32_t length);
  bool WriteTupleToExtent(Page *extent_header_page, void *tuple, uint32_t length);

  bool WriteTupleToPage(Page *extent_page, uint32_t off, void *tuple, uint32_t length);

  bool LinkPage(PageID left_id, PageID right_id);

 private:
  bool CreateExtentInFile(Page *segment_page, fileno_t data_file_no,
                          PageID *extent_header_page_id);
  std::vector<pageno_t> data_file_;
  BufferManager *buffer_manager_;
  uint32_t page_size_;
};

}  // namespace storage

#endif // SRC_STORAGE_SPACE_MANAGER_H_

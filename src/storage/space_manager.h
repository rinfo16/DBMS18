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

  void Vacuum();

  void VacuumAll();

  bool WriteTuple(PageID segment_header_pageid, void *tuple, uint32_t length);

  bool CreateSegment(PageID *segment_header_page);

 private:
  bool CreateFile(fileno_t fileno);

  bool CreateDataFile(fileno_t *fileno);

  bool AllocateExtentInSegment(Page* segment_header_page,
                               PageID *extent_header_page_id);

  bool WriteTupleToExtent(Page *segment_header_page, Page *extent_header_page,
                          void *tuple, uint32_t length);

  bool WriteTupleToPage(Page *segment_header_page, Page *extent_header_page,
                        uint32_t off, void *tuple, uint32_t length);

  bool AllocateExtentInFile(Page *segment_page, fileno_t data_file_no,
                            PageID *extent_header_page_id);

  bool LinkPage(PageID left_id, PageID right_id);

  std::vector<pageno_t> data_file_;

  BufferManager *buffer_manager_;

  uint32_t page_size_;
};

}  // namespace storage

#endif // SRC_STORAGE_SPACE_MANAGER_H_

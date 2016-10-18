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

  bool CreateSegment(PageID *segment_header_pageid /*OUT*/);

 private:
  SpaceManager();

  bool CreateFile(fileno_t fileno);

  bool CreateDataFile(fileno_t *fileno /*OUT*/);

  bool AllocateExtentInSegment(Page* segment_header_page /*INOUT*/,
                               PageID *extent_header_page_id /*OUT*/);

  bool WriteTupleToExtent(Page *segment_header_page, /*INOUT*/
                          Page *extent_header_page /*INOUT*/, const void *tuple,
                          uint32_t length);

  bool WriteTupleToPage(Page *segment_header_page /*INOUT*/,
                        Page *extent_header_page /*INOUT*/, uint32_t off,
                        const void *tuple, uint32_t length);

  bool AllocateExtentInFile(Page *segment_page /*INOUT*/, fileno_t data_file_no,
                            PageID *extent_header_page_id /*OUT*/);

  bool LinkPage(PageID left_id, PageID right_id);

  //---------------------------------------
  //
  BufferManager *buffer_manager_;

  uint32_t page_size_;
  uint32_t extent_number_per_file_;
  uint32_t page_number_per_extent_;
  std::string data_directory_;
};

}  // namespace storage

#endif // SRC_STORAGE_SPACE_MANAGER_H_

#ifndef PAGE_H_
#define PAGE_H_

#include "common/define.h"
#include "storage_define.h"
#include <memory>

static const int kPageSize = 8192;

namespace storage {

#define PAGE_HEADER_SIZE 64
#define PAGE_TAILER_SIZE 32

enum PageType {
  kPageFileHeader = 0,
  kPageSegmentHeader,
  kPageExtentHeader,
  kPageData,
  kPageIndex
};

typedef struct {
  uint32_t flip_;
  uint32_t page_type_;
  PageID pageid_;
  PageID prev_page_;
  PageID next_page_;
  uint64_t frame_ptr_;
} Page;

typedef struct {
  uint8_t data_[0];
  uint32_t flop_;
} PageTailer;

typedef struct {
  uint16_t offset_;
  uint16_t length_;
} Slot;

typedef struct {
  uint32_t free_begin_;
  uint32_t free_end_;
  uint32_t tuple_count_;
  uint32_t total_data_length_;
  //Slot slot_[0];
} DataHeader;

typedef struct {
  uint32_t page_count_;
  uint32_t data_file_count_;
  uint32_t segment_count_;
  uint32_t fileno_[0];
} SegFileHeader;

typedef struct {
  uint32_t page_count_;
  uint32_t extent_count_;
  uint8_t bits[0];
} DataFileHeader;

typedef struct {
  uint32_t extent_count_;
  PageID first_extent_header_page_id_;
  PageID last_extent_header_page_id_;
  PageID first_data_page_id_;
  PageID last_data_page_id_;
  uint32_t schema_data_length_;
  uint8_t schema_[0];

} SegmentHeader;

typedef struct {
  uint32_t page_count_;
  uint32_t used_[0];
} ExtentHeader;

}  // end namespace storage

#endif // PAGE_H_

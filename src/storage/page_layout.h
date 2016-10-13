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
  Slot slot_[0];
} DataHeader;

typedef struct {
  uint32_t page_count_;
} FileHeader;

typedef struct {
  uint32_t extent_count_;
  PageID first_data_page_;
  PageID last_data_page_;
  PageID extent_header_[0];
} SegmentHeader;

typedef struct {
  uint32_t extent_count_;
  uint8_t bits_[0];
} ExtentHeader;

DataHeader *ToDataHeader(Page *page);
SegmentHeader *ToSegmentHeader(Page *page);
FileHeader *ToFileHeader(Page *page);
ExtentHeader *ToExtentHeader(Page *page);

bool Put(Page *data_page, void *tuple, uint32_t length, slotno_t *no = NULL);
void *Get(Page *data_page, slotno_t no, uint16_t *length = NULL);

}  // end namespace storage

#endif // PAGE_H_

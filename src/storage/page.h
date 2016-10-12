#ifndef PAGE_H_
#define PAGE_H_

#include "common/define.h"
#include "storage_define.h"
#include <memory>


static const int kPageSize = 8192;

namespace storage {


typedef struct {
  uint32_t flip_;
  PageID pageid_;
  PageID prev_page_;
  PageID next_page_;
  uint32_t free_begin_;
  uint32_t free_end_;
  uint32_t tuple_count_;
} Page;

typedef struct {
  uint8_t data_[0];
  uint32_t flop_;
} PageTailer;

typedef struct {
  uint16_t offset_;
  uint16_t length_;
} SlotInPage;

}  // end namespace storage



#endif // PAGE_H_

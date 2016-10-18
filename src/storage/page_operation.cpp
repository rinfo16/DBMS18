#include "page_operation.h"
#include "common/bitmap.h"
#include <assert.h>
#include <memory.h>
#include <string.h>

namespace storage {

DataHeader *ToDataHeader(Page *page) {
  return (DataHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);

}
SegmentHeader *ToSegmentHeader(Page *page) {
  return (SegmentHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);

}

SegFileHeader *ToSegFileHeader(Page *page) {
  return (SegFileHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);
}

DataFileHeader *ToDataFileHeader(Page *page) {
  return (DataFileHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);
}

ExtentHeader *ToExtentHeader(Page *page) {
  return (ExtentHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);
}

Slot *ToFirstSlot(DataHeader *header) {
  return (Slot *) ((uint8_t*) header + sizeof(DataHeader));
}
void InitPage(Page *page, PageID id, PageType page_type, uint32_t page_size) {
  memcpy(&page->flip_, "HEAD", 4);
  page->pageid_ = id;
  page->page_type_ = page_type;
  memcpy((uint8_t*) page + page_size - PAGE_TAILER_SIZE, "TAIL", 4);
}

void InitExtentHeader(ExtentHeader *header, uint32_t extent_count_per_page) {
  header->page_count_ = extent_count_per_page;
}

void InitDataHeader(DataHeader *header, uint32_t page_size) {
  header->free_begin_ = PAGE_HEADER_SIZE + sizeof(DataHeader);
  header->free_end_ = page_size - PAGE_TAILER_SIZE;
  header->tuple_count_ = 0;
}

bool PutTuple(Page *data_page, const void *tuple, uint32_t length,
              slotno_t *slotno) {
  DataHeader *header = ToDataHeader(data_page);
  if (header->free_end_ - header->free_begin_ < length + sizeof(Slot)) {
    return false;
  }
  if (slotno != NULL) {
    *slotno = header->tuple_count_;
  }
  Slot *slot = ToFirstSlot(header);
  slot = slot + header->tuple_count_;
  header->free_begin_ += sizeof(slot);
  header->free_end_ -= length;
  header->tuple_count_++;

  slot->length_ = length;
  slot->offset_ = header->free_end_;

  memcpy(((uint8_t*) data_page + header->free_end_), tuple, length);

  return true;
}

const void *GetTuple(Page *data_page, slotno_t no, uint32_t *length) {
  DataHeader *header = ToDataHeader(data_page);
  if (no >= header->tuple_count_) {
    return NULL;
  } else {
    Slot *slot = ToFirstSlot(header);
    slot = slot + no;
    if (length != NULL) {
      *length = slot->length_;
    }
    return (uint8_t*) (data_page) + slot->offset_;
  }
}

void LinkTwoPage(Page*left, Page *right) {
  assert(left != NULL || right != NULL);
  if (left == NULL) {
    right->prev_page_ = PageID();
  }
  if (right == NULL) {
    left->next_page_ = PageID();
  }

  left->next_page_ = right->pageid_;
  right->prev_page_ = left->pageid_;
}

}  // end namespace storage

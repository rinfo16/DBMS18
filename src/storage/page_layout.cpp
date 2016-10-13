#include "page_layout.h"

namespace storage {

DataHeader *ToDataHeader(Page *page) {
  return (DataHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);

}
SegmentHeader *ToSegmentHeader(Page *page) {
  return (SegmentHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);

}

FileHeader *ToFileHeader(Page *page) {
  return (FileHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);

}

ExtentHeader *ToExtentHeader(Page *page) {
  return (ExtentHeader*) ((uint8_t*) page + PAGE_HEADER_SIZE);
}

bool Put(Page *data_page, void *tuple, uint32_t length, slotno_t *slotno) {
  DataHeader *header = ToDataHeader(data_page);
  if (header->free_end_ - header->free_end_ < length + sizeof(Slot)) {
    return false;
  }
  if (slotno != NULL) {
    *slotno = header->tuple_count_;
  }
  Slot & slot = header->slot_[header->tuple_count_];
  header->free_begin_ += sizeof(slot);
  header->free_end_ -= length;
  header->tuple_count_++;
  slot.length_ = length;
  slot.offset_ = header->free_end_;
  return true;
}

void *Get(Page *data_page, slotno_t no, uint16_t *length) {
  DataHeader *header = ToDataHeader(data_page);
  if (no >= header->tuple_count_) {
    return NULL;
  } else {
    const Slot & slot = header->slot_[no];
    if (length != NULL) {
      *length = slot.length_;
    }
    return (uint8_t*)(data_page) + slot.offset_;
  }
}

}  // end namespace storage

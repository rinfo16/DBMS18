#include "page_operation.h"
#include "common/bitmap.h"
#include "tuple_header.h"
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

TupleHeader *ToOffsetTable(DataHeader *header) {
  return (TupleHeader *) ((uint8_t*) header + sizeof(DataHeader));
}
void InitPage(Page *page, PageID id, PageType page_type, uint32_t page_size) {
  memcpy(&page->flip_, "HEAD", 4);
  page->pageid_ = id;
  page->prev_page_ = PageID();
  page->next_page_ = PageID();
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
  header->total_data_length_ = 0;
}

bool PutTuple(Page *data_page, const void *tuple, uint32_t length,
              slotno_t *slotno) {
  assert(length > 0);
  DataHeader *header = ToDataHeader(data_page);
  if (header->free_end_ - header->free_begin_ < length + sizeof(TupleHeader)) {
    return false;
  }
  if (slotno != NULL) {
    *slotno = header->tuple_count_;
  }
  TupleHeader *slot = ToOffsetTable(header);
  slot = slot + header->tuple_count_;

  header->free_begin_ += sizeof(TupleHeader);
  header->free_end_ -= length;
  header->tuple_count_++;
  header->total_data_length_ += length;

  slot->length_ = length;
  slot->offset_ = header->free_end_;

  // TODO timestamp when transaction begin
  slot->create_trans_ = 1;
  slot->delete_trans_ = 0;

  memcpy(((uint8_t*) data_page + header->free_end_), tuple, length);
  assert(header->free_begin_ <= header->free_end_);
  return true;
}

const void *GetTuple(Page *data_page, slotno_t no, uint32_t *length) {
  DataHeader *header = ToDataHeader(data_page);
  if (no >= header->tuple_count_) {
    return NULL;
  } else {
    TupleHeader *slot = ToOffsetTable(header);
    slot = slot + no;
    if (slot->delete_trans_) {
      return NULL;
    }
    if (length != NULL) {
      assert(slot->length_ > 0);
      *length = slot->length_;
    }
    return (uint8_t*) (data_page) + slot->offset_;
  }
}

bool RemoveTuple(Page *data_page, slotno_t no) {
  DataHeader *header = ToDataHeader(data_page);
  if (no >= header->tuple_count_) {
    return false;
  } else {
    TupleHeader *slot_array = ToOffsetTable(header);
    TupleHeader *slot = slot_array + no;
    // TODO transaction id or ...
    slot->delete_trans_ = 1;
    /*
     header->total_data_length_ -= slot->length_;
     --header->tuple_count_;
     if (no + 1 != header->tuple_count_) {  // not the last slot
     *slot = slot_array[header->tuple_count_ - 1];
     }*/

    return true;
  }
}

void LinkTwoPage(Page*left, Page *right) {
  assert(left != right);
  assert(!(left->pageid_.pageno_ == right->pageid_.pageno_
          && left->pageid_.fileno_ == right->pageid_.pageno_));
  assert(left != NULL && right != NULL);
  left->next_page_ = right->pageid_;
  right->prev_page_ = left->pageid_;
}

bool MoveDataToAnother(Page *src, Page *dst) {
  uint32_t length = 0;
  slotno_t no = 0;
  bool ret = true;
  const void *tuple = GetTuple(src, no, &length);
  while (tuple == NULL) {
    bool ok = PutTuple(dst, tuple, length);
    if (ok) {
      RemoveTuple(src, no);
    } else {
      ret = false;
      break;
    }
    tuple = GetTuple(src, ++no, &length);
  }
  return ret;
}

}  // end namespace storage

#include "iterator.h"
#include "stdlib.h"
#include "page_operation.h"

namespace storage {

Iterator::Iterator(relationid_t id, BufferManager *buffer_manager)
    : relation_id_(id),
      tuple_data_(NULL),
      tuple_length_(0),
      status_(kStatusInit),
      buffer_manager_(buffer_manager),
      page_(NULL),
      nth_slot_(0) {
}

Iterator::~Iterator() {
  if (page_ != NULL)
    buffer_manager_->UnfixPage(page_->pageid_);
}

bool Iterator::Get(TupleWarpper *tuple) {
  if (tuple_data_ == NULL)
    return false;

  // TODO  memory manager
  // the memory of Tuple::data_ will allocate from memory context
  void *ptr = malloc((size_t) tuple_length_);
  memcpy(ptr, tuple_data_, tuple_length_);
  TupleWarpper t((const uint8_t*) ptr, tuple_length_);
  *tuple = t;

  return true;
}

const void* Iterator::Get(uint32_t *length) {
  *length = tuple_length_;
  return tuple_data_;
}

bool Iterator::Delete() {
  return RemoveTuple(page_, nth_slot_ - 1);
}

void Iterator::SeekToFirst() {
  PageID segment_header_pageid;
  segment_header_pageid.pageno_ = 0;
  segment_header_pageid.fileno_ = 0;
  void *tuple = NULL;
  segment_header_pageid.pageno_ = relation_id_;
  Page *segment_header_page = buffer_manager_->FixPage(segment_header_pageid,
                                                       false);
  if (segment_header_page == NULL) {
    return;
  }
  SegmentHeader *segment_header = ToSegmentHeader(segment_header_page);
  PageID data_page_id = segment_header->first_data_page_id_;
  buffer_manager_->UnfixPage(segment_header_page->pageid_);

  page_ = buffer_manager_->FixPage(data_page_id, false);
  if (page_ == NULL) {
    return;
  }
  nth_slot_ = 0;
  SeekNext();
}

void Iterator::SeekToLast() {
  // TODO
}

void Iterator::Next() {
  SeekNext();
}

void Iterator::Prev() {
  // TODO
}

Status Iterator::GetStatus() {
  return status_;
}

void Iterator::SeekNext() {
  DataHeader *hdr = ToDataHeader(page_);
  const void *tuple = NULL;
  while (page_ != NULL) {
    if (nth_slot_ >= hdr->tuple_count_) {
      PageID data_page_id = page_->next_page_;
      buffer_manager_->UnfixPage(page_->pageid_);
      if (data_page_id.Invalid()) {
        page_ = NULL;
        tuple_data_ = NULL;
        tuple_length_ = 0;
        status_ = kStatusEOF;
        break;
      }

      page_ = buffer_manager_->FixPage(data_page_id, false);
      hdr = ToDataHeader(page_);
      nth_slot_ = 0;
    } else {
      tuple_data_ = GetTuple(page_, nth_slot_++, &tuple_length_);
      if (tuple_data_ != NULL) {
        status_ = kStatusOK;
        break;
      }
      nth_slot_++;
    }
  }
}
}  // namespace storage

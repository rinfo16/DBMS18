#include "relation_handler.h"
#include "page_operation.h"

namespace storage {

RelationHandler::RelationHandler(relationid_t relation_id, OpenMode mode,
                                 BufferManager *buffer_manager,
                                 SpaceManager *space_manager) {
  relation_id_ = relation_id;
  mode_ = mode;
  buffer_manager_ = buffer_manager;
  space_manager_ = space_manager;
  page_ = NULL;
  nth_slot_ = 0;
}

const void *RelationHandler::GetFirst(uint32_t *length) {
  PageID segment_header_pageid;
  segment_header_pageid.pageno_ = 0;
  segment_header_pageid.fileno_ = 0;
  void *tuple = NULL;
  segment_header_pageid.pageno_ = relation_id_;
  Page *segment_header_page = buffer_manager_->FixPage(segment_header_pageid,
                                                       false);
  if (segment_header_page == NULL) {
    return NULL;
  }
  SegmentHeader *segment_header = ToSegmentHeader(segment_header_page);
  PageID data_page_id = segment_header->first_data_page_id_;
  buffer_manager_->UnfixPage(segment_header_page->pageid_);

  page_ = buffer_manager_->FixPage(data_page_id, false);
  if (page_ == NULL) {
    return NULL;
  }
  return GetNextTuple(length);
}

const void *RelationHandler::GetNext(uint32_t *length) {
  return GetNextTuple(length);
}

bool RelationHandler::DeleteCurrent() {
  return RemoveTuple(page_, nth_slot_ - 1);
}

bool RelationHandler::Put(void *tuple, uint32_t length) {
  if (mode_ != kRelationWrite)
    return false;
  PageID id;
  id.fileno_ = 0;
  id.pageno_ = relation_id_;
// TODO  read/write meta data ...
  return space_manager_->WriteTuple(id, tuple, length);
}

bool RelationHandler::Drop() {
  return false;
}

bool RelationHandler::Create() {
  if (mode_ != kRelationCreate)
    return false;
  PageID id;
  id.fileno_ = 0;
  id.pageno_ = relation_id_;
// TODO  read/write meta data ...
  return space_manager_->CreateSegment(&id);
}
const void *RelationHandler::GetNextTuple(uint32_t *length) {
  const void *tuple = NULL;
  while (page_ != NULL) {
    DataHeader *hdr = ToDataHeader(page_);
    if (nth_slot_ >= hdr->tuple_count_) {
      PageID data_page_id = page_->next_page_;
      buffer_manager_->UnfixPage(page_->pageid_);
      if (data_page_id.Invalid()) {
        break;
      }

      page_ = buffer_manager_->FixPage(data_page_id, false);
      nth_slot_ = 0;
    }
    else  {
      tuple = GetTuple(page_, nth_slot_++, length);
      if (tuple != NULL) { // tuple was deleted
        break;
      }
    }
  }
  return tuple;
}
}   // namespace storage

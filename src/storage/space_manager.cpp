#include "space_manager.h"
#include "common/define.h"
#include "common/config.h"
#include "common/bitmap.h"
#include "file.h"
#include "page_layout.h"
#include "page_operation.h"
#include "buffer_manager.h"
#include "space_manager.h"
#include <memory.h>
#include <string.h>
#include <sstream>
#include <fstream>

namespace storage {

SpaceManager::SpaceManager(BufferManager *buffer_manager)
    : buffer_manager_(buffer_manager),
      append_(false) {
  page_size_ = config::Setting::instance().page_size_;
  extent_number_per_file_ = config::Setting::instance().extent_number_per_file_;
  data_directory_ = config::Setting::instance().data_directory_;
  page_number_per_extent_ = config::Setting::instance().page_number_per_extent_;
}

SpaceManager::~SpaceManager() {
}

bool SpaceManager::InitDB() {
  if (!Exists())
    return CreateFile(SEGMENT_DESCRIPT_FILE_NO);
  return true;
}

bool SpaceManager::CreateDataFile(fileno_t *no) {

  PageID pageid;
  pageid.fileno_ = 0;
  pageid.blockno_ = 0;

  Page *page = buffer_manager_->FixPage(pageid, false);
  if (page == NULL) {
    return false;
  }
  SegFileHeader *header = ToSegFileHeader(page);
  if ((header->data_file_count_ + 1) * sizeof(fileno_t) + PAGE_HEADER_SIZE
      + PAGE_TAILER_SIZE > page_size_) {
    return false;
  }
  header->fileno_[header->data_file_count_] = header->data_file_count_ + 1;
  if (no != NULL) {
    *no = ++header->data_file_count_;
  }
  buffer_manager_->UnfixPage(pageid);

  CreateFile(*no);
  return true;
}

bool SpaceManager::CreateFile(fileno_t fileno) {
  bool ok = false;
  std::stringstream ssm;
  ssm << data_directory_ << "/nutshell.data." << fileno;
  std::string path = ssm.str();
  File file(path);
  if (!file.Create()) {
    return false;
  }

  if (!file.Open()) {
    return false;
  }

  uint32_t page_size = page_size_;
  void *ptr = malloc(page_size);
  if (ptr == NULL) {
    return false;
  }

  memset(ptr, 0, page_size);
  PageID id;
  id.blockno_ = 0;
  id.fileno_ = fileno;
  Page* page = (Page*) ptr;
  InitPage(page, id, kPageFileHeader, page_size);

  if (fileno == 0) {
    SegFileHeader *seg_file_header = ToSegFileHeader(page);
    seg_file_header->data_file_count_ = 0;
    seg_file_header->segment_count_ = 0;
  } else {
    DataFileHeader *data_file_header = ToDataFileHeader(page);
    utils::Bitmap *bitmap = NULL;
#if 0
    bitmap = new (data_file_header->bits) utils::Bitmap(
        page_size_ - PAGE_HEADER_SIZE - PAGE_TAILER_SIZE - sizeof(utils::Bitmap)
        - sizeof(DataFileHeader));
#else
    bitmap = new (data_file_header->bits) utils::Bitmap(
        extent_number_per_file_ / 8);
#endif

    ;
    data_file_header->extent_count_ = 0;
  }

  ok = file.Write(0, page, page_size);
  free(ptr);

  return ok;
}

bool SpaceManager::CreateSegment(PageID *segment_header_page) {
  PageID seg_file_header_pageid;
  seg_file_header_pageid.blockno_ = 0;
  seg_file_header_pageid.fileno_ = 0;
  Page *page = buffer_manager_->FixPage(seg_file_header_pageid, false);
  if (page == NULL) {
    return false;
  }

  SegFileHeader *seg_file_header = ToSegFileHeader(page);

  PageID seg_header_pageid;
  seg_header_pageid.blockno_ = ++seg_file_header->page_count_;
  seg_header_pageid.fileno_ = seg_file_header_pageid.fileno_;
  Page *seg_hdr_page = buffer_manager_->FixPage(seg_header_pageid, true);
  if (seg_hdr_page) {
    InitPage(seg_hdr_page, seg_header_pageid, kPageSegmentHeader, page_size_);
    SegmentHeader *segment_header = ToSegmentHeader(seg_hdr_page);
    segment_header->extent_count_ = 0;
    segment_header->first_data_page_id_ = PageID();
    segment_header->last_data_page_id_ = PageID();
    segment_header->first_extent_header_page_id_ = PageID();
    segment_header->last_extent_header_page_id_ = PageID();
    if (!AllocateExtentInSegment(seg_hdr_page, NULL)) {
      buffer_manager_->UnfixPage(seg_hdr_page->pageid_);
      buffer_manager_->UnfixPage(page->pageid_);
      return false;
    }
  }
  if (segment_header_page != NULL)
    *segment_header_page = seg_header_pageid;
  PageGetFrame(seg_hdr_page)->SetDirty(true);
  PageGetFrame(page)->SetDirty(true);
  buffer_manager_->UnfixPage(seg_header_pageid);
  buffer_manager_->UnfixPage(seg_file_header_pageid);
  return true;
}

bool SpaceManager::DropSegment(PageID *pageid) {
  return false;
}
bool SpaceManager::AllocateExtentInSegment(Page* segment_header_page,
                                           PageID *extent_header_pageid) {
  bool ret = false;
  PageID new_externt_header_pageid;
  PageID seg_file_header_pageid;
  seg_file_header_pageid.blockno_ = 0;
  seg_file_header_pageid.fileno_ = 0;
  Page*segment_header_file_page = buffer_manager_->FixPage(
      seg_file_header_pageid, false);
  if (segment_header_file_page == NULL) {
    return false;
  }

  SegFileHeader *segment_file_header = ToSegFileHeader(
      segment_header_file_page);
  for (uint32_t i = 0; i < segment_file_header->data_file_count_; i++) {

    fileno_t file_no = segment_file_header->fileno_[i];
    if (!AllocateExtentInFile(segment_header_page, file_no,
                              &new_externt_header_pageid)) {
      ret = false;
      break;
    } else {
      ret = true;
      break;
    }

  }
  buffer_manager_->UnfixPage(seg_file_header_pageid);

  if (!ret) {
    fileno_t new_created = 0;
    if (!CreateDataFile(&new_created)) {
      return false;
    }
    ret = AllocateExtentInFile(segment_header_page, new_created,
                               &new_externt_header_pageid);
  }
  if (ret) {
    if (extent_header_pageid != NULL) {
      *extent_header_pageid = new_externt_header_pageid;
    }
  }
  return ret;
}

bool SpaceManager::AllocateExtentInFile(Page *segment_page,
                                        fileno_t data_file_no,
                                        PageID *extent_header_page_id) {
  PageID data_file_header_pageid;
  data_file_header_pageid.blockno_ = 0;
  data_file_header_pageid.fileno_ = data_file_no;
  Page *data_file_header_page = buffer_manager_->FixPage(
      data_file_header_pageid, false);
  if (data_file_header_page == NULL) {
    return false;
  }
  DataFileHeader *data_file_header = ToDataFileHeader(data_file_header_page);

  utils::Bitmap *bitmap = (utils::Bitmap*) (data_file_header->bits);
  uint32_t nth = bitmap->FindFirstZeroBit();
  if (nth >= bitmap->BitCount()) {
    buffer_manager_->UnfixPage(data_file_header_pageid);
    return false;
  }
  bitmap->SetBit(nth, true);
  PageID extent_header_pageid;
  extent_header_pageid.fileno_ = data_file_no;
  extent_header_pageid.blockno_ = 1 + nth * page_number_per_extent_;

  Page *page = buffer_manager_->FixPage(extent_header_pageid, true);
  if (page == NULL) {
    buffer_manager_->UnfixPage(data_file_header_pageid);
    return false;
  }
  InitPage(page, extent_header_pageid, kPageExtentHeader, page_size_);

  ExtentHeader *header = ToExtentHeader(page);
  InitExtentHeader(header, config::Setting::instance().page_number_per_extent_);
  SegmentHeader *segment_header = ToSegmentHeader(segment_page);
  if (segment_header->last_extent_header_page_id_.Invalid()) {
    segment_header->first_extent_header_page_id_ = page->pageid_;
    segment_header->last_extent_header_page_id_ = page->pageid_;
  } else {
    LinkPage(segment_header->last_extent_header_page_id_, page->pageid_);
    segment_header->last_extent_header_page_id_ = page->pageid_;
  }
  segment_header->extent_count_++;

  PageGetFrame(page)->SetDirty(true);
  PageGetFrame(data_file_header_page)->SetDirty(true);
  PageGetFrame(segment_page);

  buffer_manager_->UnfixPage(page->pageid_);
  buffer_manager_->UnfixPage(data_file_header_page->pageid_);
  if (extent_header_page_id != NULL) {
    *extent_header_page_id = extent_header_pageid;
  }
  return true;
}

bool SpaceManager::WriteTuple(PageID segment_header_pageid, void *tuple,
                              uint32_t length, bool bulk) {
  bool ok = false;
  if (length
      > page_size_ - PAGE_HEADER_SIZE - PAGE_TAILER_SIZE - sizeof(DataHeader)) {
    return false;
  }

  Page *segment_header_page = buffer_manager_->FixPage(segment_header_pageid,
                                                       false);
  if (segment_header_page == NULL) {
    return false;
  }
  PageID proper_pageid;
  SegmentHeader *segment_header = ToSegmentHeader(segment_header_page);
  assert(!segment_header->first_extent_header_page_id_.Invalid());

  PageID extent_header_pageid;
  if (bulk)
    extent_header_pageid = segment_header->last_extent_header_page_id_;
  else
    extent_header_pageid = segment_header->first_extent_header_page_id_;

  while (!extent_header_pageid.Invalid()) {
    Page *extent_header_page = buffer_manager_->FixPage(extent_header_pageid,
                                                        false);
    if (extent_header_page == NULL) {
      buffer_manager_->UnfixPage(segment_header_pageid);
      return false;
    }
    if (WriteTupleToExtent(segment_header_page, extent_header_page, tuple,
                           length)) {
      buffer_manager_->UnfixPage(extent_header_page->pageid_);
      ok = true;
      break;
    }
    extent_header_pageid = extent_header_page->next_page_;
    buffer_manager_->UnfixPage(extent_header_page->pageid_);
  }
  if (!ok) {
    PageID extent_header_pageid;
    if (AllocateExtentInSegment(segment_header_page, &extent_header_pageid)) {
      Page *extent_header_page = buffer_manager_->FixPage(extent_header_pageid,
                                                          false);
      if (extent_header_page != NULL) {
        if (WriteTupleToExtent(segment_header_page, extent_header_page, tuple,
                               length)) {
          ok = true;
        }
      }
      buffer_manager_->UnfixPage(extent_header_page->pageid_);
    }
    buffer_manager_->UnfixPage(segment_header_page->pageid_);
  }
  return ok;
}

bool SpaceManager::WriteTupleToExtent(Page* segment_header_page,
                                      Page* extent_header_page,
                                      const void *tuple, uint32_t length) {
  bool ok = false;
  ExtentHeader *extent_header = ToExtentHeader(extent_header_page);
  uint32_t i = 0;
  for (i = 0; i < extent_header->page_count_ - 1; i++) {
    // WHY (page count - 1) ?, the 1st page is extent header page,
    // only the following can be used to write data
    if (extent_header->used_[i] + length + sizeof(Slot)<
    page_size_ - PAGE_HEADER_SIZE - PAGE_TAILER_SIZE) {
      if (WriteTupleToPage(segment_header_page, extent_header_page, i, tuple,
                           length)) {
        ok = true;
        return ok;
      }
    }
  }
  return ok;
}

bool SpaceManager::WriteTupleToPage(Page *segment_header_page,
                                    Page* extent_header_page, uint32_t off,
                                    const void *tuple, uint32_t length) {
  bool ok;
  bool new_page = false;
  PageID data_pageid;
  data_pageid.blockno_ = extent_header_page->pageid_.blockno_ + 1 + off;
  data_pageid.fileno_ = extent_header_page->pageid_.fileno_;

  ExtentHeader *extent_header = ToExtentHeader(extent_header_page);
  if (extent_header->used_[off] == 0) {
    new_page = true;
  }
  Page *data_page = buffer_manager_->FixPage(data_pageid, new_page);
  if (new_page) {
    InitPage(data_page, data_pageid, kPageData, page_size_);
    InitDataHeader(ToDataHeader(data_page), page_size_);
    SegmentHeader *segment_header = ToSegmentHeader(segment_header_page);
    if (segment_header->last_data_page_id_.Invalid()) {
      segment_header->last_data_page_id_ = data_page->pageid_;
      segment_header->first_data_page_id_ = data_page->pageid_;
    } else {
      LinkPage(segment_header->last_data_page_id_, data_page->pageid_);
      segment_header->last_data_page_id_ = data_page->pageid_;
    }
    PageGetFrame(segment_header_page)->SetDirty(true);
    PageGetFrame(data_page)->SetDirty(true);
  }

  if (PutTuple(data_page, tuple, length)) {
    ok = true;
    if (new_page) {
      extent_header->used_[off]++;
    }
    extent_header->used_[off] += length + sizeof(Slot);
    PageGetFrame(data_page)->SetDirty(true);
    PageGetFrame(extent_header_page)->SetDirty(true);
  } else {
    ok = false;
  }
  buffer_manager_->UnfixPage(data_page->pageid_);
  return ok;
}

bool SpaceManager::LinkPage(PageID left_id, PageID right_id) {
  Page *left = buffer_manager_->FixPage(left_id, false);
  Page *right = buffer_manager_->FixPage(right_id, false);
  if (left == NULL || right == NULL) {
    if (left != NULL)
      buffer_manager_->UnfixPage(left->pageid_);
    if (right != NULL)
      buffer_manager_->UnfixPage(right->pageid_);
    return false;
  }
  LinkTwoPage(left, right);
  PageGetFrame(left)->SetDirty(true);
  PageGetFrame(right)->SetDirty(true);
  buffer_manager_->UnfixPage(left->pageid_);
  buffer_manager_->UnfixPage(right->pageid_);
  return true;
}

void SpaceManager::Vacuum() {
  PageID id;
  Page* seg_file_header_page = buffer_manager_->FixPage(id, false);
  if (seg_file_header_page == NULL) {
    return;
  }
  SegFileHeader* seg_file_header = ToSegFileHeader(seg_file_header_page);
  uint32_t seg_count = seg_file_header->segment_count_;
  for (uint32_t i = 0; i < seg_count; i++) {
    VacuumSegment(i);
  }
}

void SpaceManager::VacuumSegment(segmentno_t no) {
  PageID segment_pageid;
  segment_pageid.fileno_ = SEGMENT_DESCRIPT_FILE_NO;
  segment_pageid.blockno_ = no;
  Page* segment_page = buffer_manager_->FixPage(segment_pageid, false);
  if (segment_page == NULL) {
    return;
  }
  SegmentHeader *segment_header = ToSegmentHeader(segment_page);
  PageID data_pageid = segment_header->first_data_page_id_;
  Page *data_page = buffer_manager_->FixPage(data_pageid, false);

  while (data_page) {
    bool recycle = false;
    bool shrink = ShrinkPage(data_page);
    if (shrink) {
      // next page id should assign before recycle page
      // recycle page will re-link previous and next page
      data_pageid = data_page->next_page_;
      DataHeader* data_header = ToDataHeader(data_page);
      if (data_header->tuple_count_ == 0) {
        recycle = RecyclePage(data_page);
      }
    }
    buffer_manager_->UnfixPage(data_page->pageid_);
    if (data_pageid.Invalid()) {
      break;
    }
    if (recycle) {
      // TODO ...
    }
    data_page = buffer_manager_->FixPage(data_pageid, false);
  }

  buffer_manager_->UnfixPage(segment_page->pageid_);
}

void SpaceManager::VacuumAll() {

}

bool SpaceManager::ShrinkPage(Page *page) {
  DataHeader *header = ToDataHeader(page);
  uint32_t data_region_size = page_size_ - header->free_end_ - PAGE_TAILER_SIZE;
  if (data_region_size == 0) {
    return false;
  }
  if ((double) (header->total_data_length_) / (double) (data_region_size)
      > 0.6) {
    return false;
  }

  void *ptr = malloc(data_region_size);
  Slot *slot = ToFirstSlot(header);
  header->total_data_length_ = 0;
  header->free_end_ = page_size_ - PAGE_TAILER_SIZE;
  uint32_t free_end_previous = header->free_end_;
  memcpy(ptr, (uint8_t*) page + free_end_previous, data_region_size);
  for (pageno_t no = 0; no < header->tuple_count_; no++) {
    Slot &s = slot[no];
    memcpy((uint8_t *) page + header->free_end_ + s.length_,
           (uint8_t*) ptr + s.length_ - free_end_previous, s.length_);

    // update header ..
    header->free_end_ += s.length_;
    header->total_data_length_ += s.length_;

    // update slot
    s.offset_ = header->free_end_;
  }
  free(ptr);
  PageGetFrame(page)->SetDirty(true);

  return true;
}

bool SpaceManager::RecyclePage(Page *data_page) {
  PageID extent_pageid = data_page->pageid_;
  extent_pageid.blockno_ = (extent_pageid.blockno_ - 1)
      / page_number_per_extent_ * page_number_per_extent_;
  Page *extent_header_page = buffer_manager_->FixPage(extent_pageid, false);
  assert(extent_header_page);
  ExtentHeader *extent_header = ToExtentHeader(extent_header_page);
  uint32_t off = data_page->pageid_.blockno_
      - extent_header_page->pageid_.blockno_ - 1;
  extent_header->used_[off] = 0;
  PageGetFrame(extent_header_page)->SetDirty(true);

  // data in this page cannot be usee before it has been written new data ...
  PageGetFrame(data_page)->SetDirty(false);
  LinkPage(data_page->prev_page_, data_page->next_page_);
  buffer_manager_->UnfixPage(extent_pageid);
  return true;
}

bool SpaceManager::Exists() {
  std::stringstream ssm;
  ssm << data_directory_ << "/nutshell.data." << 0;
  std::ifstream infile(ssm.str().c_str());
  return infile.good();
}

}  // namespace storage

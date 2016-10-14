#include "space_manager.h"
#include <memory.h>
#include <string.h>
#include <sstream>
#include "file.h"
#include "common/config.h"
#include "common/bitmap.h"
#include "page_layout.h"
#include "page_operation.h"

namespace config {
extern Setting gSetting;
}

namespace storage {

SpaceManager::SpaceManager(BufferManager *buffer_manager)
    : buffer_manager_(buffer_manager) {
  // TODO Auto-generated constructor stub
}

SpaceManager::~SpaceManager() {
  // TODO Auto-generated destructor stub
}

bool SpaceManager::InitDB() {
  return CreateFile(SEGMENT_DESCRIPT_FILE_NO) && CreateFile(DATA_FILE_NO);
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
      + PAGE_TAILER_SIZE > config::gSetting.page_size_) {
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
  std::stringstream ssm;
  ssm << config::gSetting.data_directory_ << "/nutshell.data." << fileno;
  std::string path = ssm.str();
  File file(path);
  if (!file.Create()) {
    return false;
  }

  if (!file.Open()) {
    return false;
  }

  uint32_t page_size = config::gSetting.page_size_;
  void *ptr = malloc(page_size);
  if (ptr == NULL) {
    return false;
  }

  memset(ptr, 0, page_size);
  Page* page = (Page*) ptr;
  page->flip_ = FLIP_NUMBER;
  page->page_type_ = kPageFileHeader;
  page->pageid_.blockno_ = 0;
  page->pageid_.fileno_ = fileno;

  if (fileno == 0) {
    SegFileHeader *seg_file_header = ToSegFileHeader(page);
    seg_file_header->data_file_count_ = 0;
    seg_file_header->segment_count_ = 0;
  } else {
    DataFileHeader *data_file_header = ToDataFileHeader(page);
    data_file_header->extent_count_ = 0;
  }

  if (file.Write(0, page, page_size)) {
    free(ptr);
    return false;
  } else {
    free(ptr);
  }
  file.Close();

  return true;
}
bool SpaceManager::CreateSegment(PageID *segment_header_page) {
  PageID file_header_pageid;
  file_header_pageid.fileno_ = 0;
  file_header_pageid.blockno_ = 0;
  Page *page = buffer_manager_->FixPage(file_header_pageid, false);
  if (page == NULL) {
    return false;
  }

  SegFileHeader *seg_file_header = ToSegFileHeader(page);

  PageID seg_header_pageid;
  seg_header_pageid.blockno_ = seg_file_header->page_count_++;
  seg_header_pageid.fileno_ = file_header_pageid.fileno_;
  Page *seg_hdr_page = buffer_manager_->FixPage(seg_header_pageid, true);
  if (segment_header_page) {
    InitPage(seg_hdr_page, seg_header_pageid, kPageSegmentHeader);
    SegmentHeader *segment_header = ToSegmentHeader(seg_hdr_page);
    segment_header->extent_count_ = 0;
    if (!CreateExtentInSegment(seg_hdr_page, NULL)) {
      buffer_manager_->UnfixPage(seg_header_pageid);
      buffer_manager_->UnfixPage(file_header_pageid);
      return false;
    }
  }
  buffer_manager_->UnfixPage(seg_header_pageid);
  buffer_manager_->UnfixPage(file_header_pageid);
  return true;

}

bool SpaceManager::CreateExtentInSegment(Page* segment_header_page,
                                         PageID *extent_header_page_id) {
  bool ret = false;
  PageID new_externt_header_pageid;
  PageID seg_file_header_pageid;
  Page*segment_header_file_page = buffer_manager_->FixPage(
      seg_file_header_pageid, false);
  if (segment_header_file_page == NULL) {
    return false;
  }

  SegFileHeader *seg_file_header = ToSegFileHeader(segment_header_file_page);
  for (uint32_t i = 0; i < seg_file_header->data_file_count_; i++) {

    fileno_t file_no = seg_file_header->fileno_[i];
    if (!CreateExtentInFile(segment_header_page, file_no,
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
    ret = CreateExtentInFile(segment_header_page, new_created,
                             &new_externt_header_pageid);
  }
  if (ret) {
    SegmentHeader *header = ToSegmentHeader(segment_header_page);
    header->extent_count_++;
    header->first_data_page_id_ = PageID();
  }
  return ret;
}

bool SpaceManager::CreateExtentInFile(Page *segment_page, fileno_t data_file_no,
                                      PageID *extent_header_page_id) {
  PageID data_file_header_pageid;
  Page *data_file_header_page = buffer_manager_->FixPage(
      data_file_header_pageid, false);
  if (data_file_header_page == NULL) {
    return false;
  }
  DataFileHeader *data_file_header = ToDataFileHeader(data_file_header_page);

  // TODO find a 0 in bitmap ...
  utils::Bitmap *bitmap = (utils::Bitmap*) (data_file_header->bits);
  uint32_t nth = bitmap->FindZero(0, 1);
  if (nth >= bitmap->BitCount()) {
    buffer_manager_->UnfixPage(data_file_header_pageid);
    return false;
  }

  PageID extent_header_pageid;
  extent_header_pageid.fileno_ = data_file_no;
  extent_header_pageid.blockno_ = 1
      + nth * config::gSetting.page_number_per_extent_;

  Page *page = buffer_manager_->FixPage(extent_header_pageid, true);
  if (page == NULL) {
    buffer_manager_->UnfixPage(data_file_header_pageid);
    return false;
  }
  InitPage(page, extent_header_pageid, kPageExtentHeader);

  page->pageid_ = extent_header_pageid;
  page->page_type_ = kPageExtentHeader;
  ExtentHeader *header = ToExtentHeader(page);
  InitExtentHeader(header, config::gSetting.page_number_per_extent_);
  SegmentHeader *segment_header = ToSegmentHeader(segment_page);
  if (segment_header->last_extent_header_page_id_.Invalid()) {
    segment_header->first_extent_header_page_id_ = page->pageid_;
    segment_header->last_extent_header_page_id_ = page->pageid_;
  } else {
    LinkPage(segment_header->last_extent_header_page_id_, page->pageid_);
    segment_header->last_extent_header_page_id_ = page->pageid_;
  }
  buffer_manager_->UnfixPage(extent_header_pageid);
  buffer_manager_->UnfixPage(data_file_header_pageid);
  return true;
}

bool SpaceManager::CreateDataPageInSegment(Page *segment_header_page,
                                           PageID *new_page_id) {
  bool ok = false;
  PageID new_data_pageid;
  SegmentHeader *segment_header = ToSegmentHeader(segment_header_page);
  Page *extent_header_page = buffer_manager_->FixPage(
      segment_header->first_extent_header_page_id_, false);
  while (true) {
    if (CreateDataPageInExtent(extent_header_page, &new_data_pageid)) {
      buffer_manager_->UnfixPage(extent_header_page->pageid_);
      ok = true;
      break;
    }
    if (extent_header_page->next_page_.Invalid()) {
      buffer_manager_->UnfixPage(extent_header_page->pageid_);
      break;
    } else {
      PageID next_id = extent_header_page->next_page_;
      buffer_manager_->UnfixPage(extent_header_page->pageid_);
      extent_header_page = buffer_manager_->FixPage(next_id, false);
      if (extent_header_page == NULL) {
        break;
      }
    }
  }
  if (!ok) {
    // Create a new extent, then allocate a new data page in this extent
    // ...
    PageID extent_header_pageid;
    if (CreateExtentInSegment(segment_header_page, &extent_header_pageid)) {
      ok = false;
    } else {
      extent_header_page = buffer_manager_->FixPage(extent_header_pageid,
                                                    false);
      if (extent_header_page == NULL) {
        ok = false;
      } else {
        ok = CreateDataPageInExtent(extent_header_page, false);

      }
      buffer_manager_->UnfixPage(extent_header_pageid);
    }
  }

  if (ok) {
    if (!segment_header->first_data_page_id_.Invalid()) {
      assert(segment_header->last_data_page_id_.Invalid());
      segment_header->first_data_page_id_ = new_data_pageid;
      segment_header->last_data_page_id_ = new_data_pageid;
    } else {
      LinkPage(segment_header->last_data_page_id_, new_data_pageid);
      segment_header->last_data_page_id_ = new_data_pageid;
    }

  }
  return ok;
}

bool SpaceManager::CreateDataPageInExtent(Page *extent_header_page,
                                          PageID *new_page_id) {
  ExtentHeader *extent_header = ToExtentHeader(extent_header_page);
  utils::Bitmap *bitmap = (utils::Bitmap*) (extent_header->bits_);
  uint32_t n = bitmap->FindZero(0, 1);
  if (n >= bitmap->BitCount()) {
    return false;
  }
  PageID new_data_page_id = extent_header_page->pageid_;
  new_data_page_id.blockno_ = new_data_page_id.blockno_ + 1 + n;

  Page* new_data_page = buffer_manager_->FixPage(new_data_page_id, true);
  if (new_data_page == NULL) {
    return false;
  }
  DataHeader *header = ToDataHeader(new_data_page);
  InitPage(new_data_page, new_data_page_id, kPageData);
  InitDataHeader(header, config::gSetting.page_size_);
  buffer_manager_->UnfixPage(new_data_page_id);
  *new_page_id = new_data_page_id;
  extent_header->page_count_++;
  return true;
}

bool SpaceManager::FindSpace(PageID segment_header_page, size_t length,
                             PageID *data_page) {
  return true;
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
  buffer_manager_->UnfixPage(left->pageid_);
  buffer_manager_->UnfixPage(right->pageid_);
  return true;
}

}
// namespace storage

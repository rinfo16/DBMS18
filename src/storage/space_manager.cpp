#include "space_manager.h"
#include <memory.h>
#include <string.h>
#include <sstream>
#include "file.h"
#include "common/config.h"
#include "common/bitmap.h"
#include "page_layout.h"

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
    memset(seg_hdr_page, 0, config::gSetting.page_size_);
    SegmentHeader *segment_header = ToSegmentHeader(seg_hdr_page);
    segment_header->extent_count_ = 0;
    if (!CreateExtent(page, seg_hdr_page, NULL)) {
      buffer_manager_->UnfixPage(seg_header_pageid);
      buffer_manager_->UnfixPage(file_header_pageid);
      return false;
    }
  }
  buffer_manager_->UnfixPage(seg_header_pageid);
  buffer_manager_->UnfixPage(file_header_pageid);
  return true;

}

bool SpaceManager::CreateExtent(Page*segment_header_file_page,
                                Page* segment_header_page,
                                PageID *extent_header_page_id) {
  bool ret = false;
  SegFileHeader *seg_file_header = ToSegFileHeader(segment_header_file_page);
  for (uint32_t i = 0; i < seg_file_header->data_file_count_; i++) {

    fileno_t file_no = seg_file_header->fileno_[i];
    if (!CreateExtentInFile(segment_header_page, file_no,
                            extent_header_page_id)) {
      return false;
    } else {
      ret = true;
      break;
    }

  }
  if (!ret) {
    fileno_t new_created = 0;
    if (!CreateDataFile(&new_created)) {
      return false;
    }
    ret = CreateExtentInFile(segment_header_page, new_created,
                             extent_header_page_id);
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
  if (nth >= bitmap->Length()) {
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
  memset(page, 0, config::gSetting.page_size_);
  page->pageid_ = extent_header_pageid;
  page->page_type_ = kPageExtentHeader;
  ExtentHeader *header = ToExtentHeader(page);
  header->page_count_ = 0;

  SegmentHeader *segment_header = ToSegmentHeader(segment_page);
  if (!segment_header->first_extent_header_page_id_.Invalid()) {
    Page *first_page = buffer_manager_->FixPage(
        segment_header->first_extent_header_page_id_, false);
    LinkPage(first_page, page);
  } else {
    segment_header->first_extent_header_page_id_ = page->pageid_;
  }
  buffer_manager_->UnfixPage(extent_header_pageid);
  buffer_manager_->UnfixPage(data_file_header_pageid);
  return true;
}

bool SpaceManager::FindSpace(PageID segment_header_page, size_t length,
                             PageID *data_page_) {
  return true;
}

}
// namespace storage

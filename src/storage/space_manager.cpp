#include "space_manager.h"
#include <memory.h>
#include <string.h>
#include "file.h"
#include "common/config.h"

namespace config {
extern Setting gSetting;
}

namespace storage {

SpaceManager::SpaceManager() {
  // TODO Auto-generated constructor stub

}

SpaceManager::~SpaceManager() {
  // TODO Auto-generated destructor stub
}

bool SpaceManager::InitDB() {
  return CreateFile(SEGMENT_DESCRIPT_FILE_NO) && CreateFile(DATA_FILE_NO);
}

bool SpaceManager::CreateFile(fileno_t fileno) {
  std::string path = config::gSetting.data_directory_ + "/" + "nutshell.data.0";

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

  FileHeader *header = (FileHeader*) ((uint8_t*) ptr + sizeof(Page));
  header->page_count_ = 1;

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
  return true;
}

bool SpaceManager::CreateExtent(PageID segment_header_page_id,
                                PageID *extent_header_page_id) {
  PageControl control;
  if (!buffer_manager_->FixPage(segment_header_page_id, &control)) {
    return false;
  }

  Page* segment_header_page = control.page_;
  SegmentHeader *segment_header = NULL;
  // TODO ...

  for (size_t i = 0; i < data_file_.size(); i++) {
    fileno_t fileno = data_file_[i];
    PageID pageid;
    pageid.fileno_ = fileno;
    pageid.blockno_ = FILE_HEADER_PAGE_NO;

    control.is_new_ = false;
    if (!buffer_manager_->FixPage(pageid, &control)) {
      buffer_manager_->UnfixPage(segment_header_page_id);
      return false;
    }

    // TODO
    // 1. Write the first extent ..
    // 2. update the segment header page ..
    buffer_manager_->UnfixPage(segment_header_page_id);
    buffer_manager_->UnfixPage(pageid);

    *extent_header_page_id = pageid;
    return true;
  }
  DONE:

  return true;
}

bool SpaceManager::FindSpace(PageID segment_header_page, size_t length,
                             PageID *data_page_) {
  return true;
}

}
// namespace storage

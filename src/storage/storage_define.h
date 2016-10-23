#ifndef STORAGE_DEFINE_H_
#define STORAGE_DEFINE_H_

#include "common/define.h"
#include "common/page_id.h"

typedef size_t buffer_index_t;



struct SegmentDescripter {
  segmentno_t segmentno_;
  PageID first_page_id_;
};

#endif // STORAGE_DEFINE_H_

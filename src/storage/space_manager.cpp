#include "space_manager.h"

namespace storage {

SpaceManager::SpaceManager() {
  // TODO Auto-generated constructor stub

}

SpaceManager::~SpaceManager() {
  // TODO Auto-generated destructor stub
}

bool SpaceManager::CreateSegment(PageID *segment_header_page) {
  return true;
}

bool SpaceManager::CreateExtent(PageID segment_header_page,
                                PageID *extent_header_page) {
  return true;
}

bool SpaceManager::FindSpace(PageID segment_header_page, size_t length, PageID *data_page_)
{
  return true;
}

}
  // namespace storage

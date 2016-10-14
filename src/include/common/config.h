#ifndef CONFIG_H_
#define CONFIG_H_

#include "common/define.h"
#include "common/singleton.hpp"
#include <string>

namespace config {
struct Setting : utils::Singleton<Setting>{
  Setting() {
    page_size_ = PAGE_SIZE;
    page_number_per_extent_ = PAGE_NUMBER_PER_EXTENT;
    buffer_pool_size_ = BUFFER_POOL_SIZE;
    data_directory_ = DATA_DIRECTORY;
    max_page_number_per_file_ = MAX_PAGE_NUMBER_PER_FILE;
    max_extent_count_ = MAX_EXTENT_COUNT;
  }

  uint32_t page_size_;
  uint32_t page_number_per_extent_;
  uint32_t buffer_pool_size_;
  uint32_t max_page_number_per_file_;
  uint32_t max_extent_count_;
  std::string data_directory_;
};

}  // end namespace config

#endif // CONFIG_H_

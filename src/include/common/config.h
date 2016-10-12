#ifndef CONFIG_H_
#define CONFIG_H_

#include "common/define.h"
#include <string>

namespace config {
struct Setting {
  Setting() {
    page_size_ = PAGE_SIZE;
    page_number_per_extent_ = PAGE_NUMBER_PER_EXTENT;
    buffer_pool_size_ = BUFFER_POOL_SIZE;
    data_directory_ = DATA_DIRECTORY;
    max_page_number_per_file_ = MAX_PAGE_NUMBER_PER_FILE;
  }

  uint32_t page_size_;
  uint32_t page_number_per_extent_;
  uint32_t buffer_pool_size_;
  uint32_t max_page_number_per_file_;
  std::string data_directory_;
};

}  // end namespace config

#endif // CONFIG_H_

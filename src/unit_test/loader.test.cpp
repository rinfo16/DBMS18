#include "storage.test.h"
#include <boost/program_options.hpp>
#include "common/config.h"
#include "storage/storage_service.h"

int main(int argc, const char *argv[]) {
  config::Setting::instance().buffer_pool_size_ = 8192 * 100;
  config::Setting::instance().data_directory_ = "/tmp";
  config::Setting::instance().extent_number_per_file_ = 20;
  config::Setting::instance().page_number_per_extent_ = 100;

  std::string path;
  int rows = 1000000;
  std::string table = "PRODUCT";

  path = "/tmp/data.csv";
  create_csv(rows, path);

  storage::Storage::instance().Start();
  create_table(table);
  import_from_csv(table, path);
  storage::Storage::instance().Stop();

  storage::Storage::instance().Start();
  path = path + ".export.csv";
  export_to_csv(table, path);
  storage::Storage::instance().Stop();

  return 0;
}


#include "storage.test.h"
#include <iostream>
#include <boost/program_options.hpp>
#include "storage/storage_service.h"

using namespace boost::program_options;

int main(int argc, const char *argv[]) {
  std::string path;
  int rows = 0;
  std::string table;
  bool create_test_csv_file = false;
  bool import_csv = false;
  bool export_csv = false;

  try {
    options_description desc { "Options" };
    desc.add_options()("--help,-h", "Help screen\n"
                       "--csv --path=PATH --rows=ROW\n"
                       "--import --table=TABLE --path=PATH\n"
                       "--export --table=TABLE --path=PATH\n")(
        "rows", value<int>()->default_value(100000), "rows to output")(
        "path", value<std::string>(), "csv file path")("csv", "create csv file")(
        "import", "import csv file")("export", "export csv file")(
        "table", value<std::string>(), "table name");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
      std::cout << desc << '\n';
    if (vm.count("rows")) {
      std::cout << vm["rows"].as<int>() << std::endl;
      rows = vm["rows"].as<int>();
    }
    if (vm.count("path")) {
      std::cout << vm["path"].as<std::string>() << std::endl;
      path = vm["path"].as<std::string>();
    }
    if (vm.count("csv")) {
      std::cout << "create test data ..." << std::endl;
      create_test_csv_file = true;
    }
    if (vm.count("import")) {
      import_csv = true;
    }
    if (vm.count("export")) {
      export_csv = true;
    }
    if (vm.count("table")) {
      table = vm["table"].as<std::string>();
    }
  } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
  }

  if (create_test_csv_file && rows != 0 && path != "") {
    create_csv(rows, path);
  } else if (import_csv && path != "" && table != "") {
    storage::Storage::instance().Start();
    create_table(table);
    import_from_csv(table, path);
    storage::Storage::instance().Stop();
  } else if (export_csv && path != "" && table != "") {
    storage::Storage::instance().Start();
    export_to_csv(table, path);
    storage::Storage::instance().Stop();
  }
}

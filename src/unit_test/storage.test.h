#ifndef SRC_UNIT_TEST_STORAGE_TEST_H_
#define SRC_UNIT_TEST_STORAGE_TEST_H_

#include <string>
#include "common/define.h"


#define CSV_FILE_PATH "/tmp/products.csv"
#define NAME_LENGTH 128
#define TEST_ROWS 50000
#define TABLE_NAME "T1"

struct Product {
  Product()
      : name(""),
        qty(0),
        price(0.0f),
        date(0) {
  }
  Product(std::string name_, uint64_t qty_, double_t price_, int64_t date)
      : name(name_),
        qty(qty_),
        price(price_),
        date(date) {
  }
  std::string name;
  int64_t qty;
  double_t price;
  int64_t date;
};

void gen_random(char *s, const int len);
int create_csv(int test_rows, std::string path);
int create_table(std::string table_name);
int import_from_csv(std::string table, std::string path);
int export_to_csv(std::string table, std::string path) ;

#endif // SRC_UNIT_TEST_STORAGE_TEST_H_

#include "storage/loader.h"
#include "common/minicsv.h"
#include "common/define.h"
#include "storage/storage_service.h"
#include <iostream>
#include <math.h>
#include <sstream>

#define CSV_FILE_PATH "/tmp/products.csv"
#define NAME_LENGTH 128
#define TEST_ROWS 50000
using namespace std;

void gen_random(char *s, const int len) {
  static const char alphanum[] = "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

  for (int i = 0; i < len; ++i) {
    s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  s[len] = 0;
}

struct Product {
  Product()
      : name(""),
        qty(0),
        price(0.0f) {
  }
  Product(std::string name_, uint64_t qty_, double_t price_)
      : name(name_),
        qty(qty_),
        price(price_) {
  }
  std::string name;
  int64_t qty;
  double_t price;
};

int create_csv() {
  mini::csv::ofstream os(CSV_FILE_PATH);
  os.set_delimiter(',', "$$");
  os.enable_surround_quote_on_str(true, '\"');
  if (!os.is_open()) {
    return -1;
  }

  char buffer[1024];

  for (int i = 0; i < TEST_ROWS; i++) {

    gen_random(buffer, rand() % (NAME_LENGTH / 2));

    stringstream ssm;
    ssm << "NAME__" << i << "__" << buffer << "__" << i;
    Product product(ssm.str().c_str(), rand() % 20000, (double) rand() / (double) 10000);
    os << product.name << product.qty << product.price << NEWLINE;
  }

  os.flush();
  os.close();
  return true;
}

int test_load() {
  TableSchema sch;

  Column column;
  column.data_type_ = kDTVarchar;
  column.is_null_ = false;
  column.length_ = NAME_LENGTH;
  column.name_ = "NAME";
  sch.column_list_.push_back(column);

  column.data_type_ = kDTInteger;
  column.is_null_ = false;
  column.name_ = "QTY";
  sch.column_list_.push_back(column);

  column.data_type_ = kDTFloat;
  column.is_null_ = false;
  column.name_ = "PRICE";
  sch.column_list_.push_back(column);

  sch.name_ = "PRODUCT";

  if (!storage::Storage::instance().CreateRelation(sch)) {
    return false;
  }

  std::cout << "create schema " << sch.name_ << std::endl;
  // write to relation 1

  std::string csv = CSV_FILE_PATH;
  storage::Loader *loader = new storage::Loader(CSV_FILE_PATH, sch.name_);
  bool ok = loader->Load();
  if (!ok) {
    std::cout << "load failed ..." << std::endl;
    return -1;
  }

  std::cout << "load finish ..." << std::endl;

  string csv_new = CSV_FILE_PATH;
  csv_new = csv_new + ".new";
  mini::csv::ofstream os_new(csv_new.c_str());
  os_new.set_delimiter(',', "$$");
  os_new.enable_surround_quote_on_str(true, '\"');
  if (!os_new.is_open()) {
    return -1;
  }

  Relation *rel = storage::Storage::instance().GetMetaDataManager()->GetRelationByName(sch.name_);
  TupleDesc desc = rel->ToTupleDesc();
  storage::IteratorInterface *iter = storage::Storage::instance().NewIterator(sch.name_);
  iter->SeekToFirst();
  uint32_t slot_length = sizeof(Slot) * desc.column_count_;
  while (iter->GetStatus() == storage::kStatusOK)
  {
    uint32_t length = 0;
    Product product;
    Tuple *t = (Tuple*)iter->Get(&length);
    std::string s((const char*)t + slot_length, length - slot_length);
    std::cout << s << std::endl;
    for (int i = 0; i < desc.column_count_; i++)
    {
      Slot *slot = t->GetSlot(i, &desc);
      switch(desc.GetType(i)) {
         case kDTInteger:
           product.qty = *reinterpret_cast<const int64_t*>(t->GetValue(slot->offset_));
           break;
         case kDTFloat:
           product.price = *reinterpret_cast<const double_t*>(t->GetValue(slot->offset_));
           break;
         case kDTVarchar:
           product.name = std::string(reinterpret_cast<const char*>(t->GetValue(slot->offset_)), slot->length_);
           break;
         case kDTDate:
           break;
           // TODO ...

           break;
         default:
           assert(false);
           break;
       }
    }
    iter->Next();
    os_new << product.name << product.qty << product.price << NEWLINE;
  }
  os_new.flush();
  os_new.close();
  return 0;
}

int main() {
  std::cout << "writing test csv file ..." << std::endl;
  create_csv();

  storage::Storage::instance().Start();

  test_load();
}

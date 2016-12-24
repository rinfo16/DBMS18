#include "storage.test.h"
#include <iostream>
#include <math.h>
#include <sstream>
#include "common/minicsv.h"
#include "common/define.h"
#include "common/datetime.h"
#include "storage/loader.h"
#include "storage/storage_service_interface.h"


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



int create_csv(int test_rows, std::string path) {
  mini::csv::ofstream os(path.c_str());
  os.set_delimiter(',', "$$");
  os.enable_surround_quote_on_str(true, '\"');
  if (!os.is_open()) {
    return -1;
  }

  char buffer[1024];

  for (int i = 0; i < test_rows; i++) {

    gen_random(buffer, rand() % (NAME_LENGTH / 2));

    stringstream ssm;
    ssm << "NAME__" << i << "__" << buffer << "__" << i;
    Product product(ssm.str().c_str(), rand() % 20000,
                    (double) rand() / (double) 10000, rand());
    os << product.qty << product.price << product.name <<  NEWLINE;
  }

  os.flush();
  os.close();
  return true;
}

int create_table(std::string table_name) {
  TableSchema sch;

  ColumnSchema column;
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

  column.data_type_ = kDTDate;
  column.is_null_ = false;
  column.name_ = "DATE";
  sch.column_list_.push_back(column);

  sch.name_ = table_name;

  if (!storage::StorageServiceInterface::Instance()->CreateRelation(sch)) {
    return false;
  }

  std::cout << "create schema " << sch.name_ << std::endl;
  // write to relation 1
  return 0;
}

int import_from_csv(std::string table, std::string path) {
  storage::Loader *loader = new storage::Loader(path, table);
  bool ok = loader->Load();
  if (!ok) {
    std::cout << "load failed ..." << std::endl;
    return -1;
  }
  delete loader;
  std::cout << "load finish ..." << std::endl;
  return 0;
}

int export_to_csv(std::string table, std::string path) {
  mini::csv::ofstream os2(path.c_str());
  os2.set_delimiter(',', "$$");
  os2.enable_surround_quote_on_str(true, '\"');
  if (!os2.is_open()) {
    return -1;
  }
  Relation *rel = storage::StorageServiceInterface::Instance()->GetRelationByName(table);
  RowDesc desc = rel->ToDesc();
  storage::IteratorInterface *iter = storage::StorageServiceInterface::Instance()->NewIterator(
      table);
  iter->SeekToFirst();
  size_t slot_length = sizeof(Slot) * desc.GetColumnCount();
  int i = 0;
  while (iter->GetStatus() == storage::kStatusOK) {
    uint32_t length = 0;
    Product product;
    RawTuple *t = (RawTuple*) iter->Get(&length);
#if OUTPUT
    std::string s((const char*) t + slot_length, length - slot_length);
    std::cout << s << std::endl;
#endif
    for (int i = 0; i < desc.GetColumnCount(); i++) {
      Slot *slot = t->GetSlot(i);
      switch (desc.GetColumnDesc(i).data_type_) {
        case kDTInteger:
          product.qty = *reinterpret_cast<const int64_t*>(t->GetValue(
              slot->offset_));
          break;
        case kDTFloat:
          product.price = *reinterpret_cast<const double_t*>(t->GetValue(
              slot->offset_));
          break;
        case kDTVarchar:
          product.name = std::string(
              reinterpret_cast<const char*>(t->GetValue(slot->offset_)),
              slot->length_);
          break;
        case kDTDate:
          break;
        default:
          assert(false);
          break;
      }
    }

    os2 << product.name << product.qty << product.price << NEWLINE;

    if (i % 100000 && i != 0)
      std::cout << "export " << i++ << " rows ..." << std::endl;
    iter->Next();
  }
  os2.flush();
  os2.close();

  return 0;
}



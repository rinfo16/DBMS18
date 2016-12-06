#include "storage/storage_service.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void test1() {
  storage::Storage::instance().Start();
  TableSchema sch1;
  TableSchema sch2;
  TableSchema sch3;

  ColumnSchema column;
  column.data_type_ = kDTVarchar;
  column.is_null_ = false;
  column.length_ = 32;
  column.name_ = "COLUMN_A";
  sch1.column_list_.push_back(column);

  column.data_type_ = kDTFloat;
  column.is_null_ = false;
  column.name_ = "COLUMN_B";
  sch1.column_list_.push_back(column);

  column.data_type_ = kDTInteger;
  column.is_null_ = false;
  column.name_ = "COLUMN_C";
  sch1.column_list_.push_back(column);

  column.data_type_ = kDTFloat;
  column.is_null_ = false;
  column.name_ = "COLUMN_D";
  sch1.column_list_.push_back(column);

  column.data_type_ = kDTInteger;
  column.is_null_ = false;
  column.name_ = "COLUMN_E";
  sch1.column_list_.push_back(column);

  sch3 = sch2 = sch1;

  sch1.name_ = "TABLE_1";
  sch2.name_ = "TABLE_2";
  sch3.name_ = "TABLE_3";

  bool ret = storage::Storage::instance().CreateRelation(sch1);
  ret = storage::Storage::instance().CreateRelation(sch2);
  ret = storage::Storage::instance().CreateRelation(sch3);
  storage::Storage::instance().Stop();
}

void test2() {
  storage::Storage::instance().Start();

  // write to relation 1
  storage::WriteBatchInterface *write_batch = storage::Storage::instance()
      .NewWriteBatch("TABLE_1");

  char buff[64];
  for (int i = 0; i < 50000; i++) {
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "TUPLE_%d", i);
    sprintf(buff, "TUPLE_%d", i);
    if (i == 35800)
      int t = 0;
    if (i % 100 == 0)
      std::cout << buff << std::endl;
    TupleWarpper tuple((const uint8_t*) buff, sizeof(buff));
    write_batch->Put(&tuple);
  }
  storage::Storage::instance().DeleteIOObject(write_batch);

  // write to relation 2
  write_batch = storage::Storage::instance().NewWriteBatch("TABLE_2");

  for (int i = 0; i < 50000; i++) {
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "TUPLE_%d", i);
    if (i % 100 == 0)
      std::cout << buff << std::endl;
    TupleWarpper tuple((uint8_t*) buff, sizeof(buff));
    write_batch->Put(&tuple);
  }
  storage::Storage::instance().DeleteIOObject(write_batch);
  storage::Storage::instance().Stop();
}

void test3() {
  storage::Storage::instance().Start();
  storage::IteratorInterface *iter = storage::Storage::instance().NewIterator(
      "TABLE_1");

  uint32_t length = 0;
  iter->SeekToFirst();
  while (iter->GetStatus() != storage::kStatusEOF) {
    TupleWarpper tuple;
    iter->Get(&tuple);
    std::string str((const char *) tuple.Data(), tuple.Size());
    std::cout << str << std::endl;
    free((void*) tuple.Data());
    iter->Next();
  }
  storage::Storage::instance().DeleteIOObject(iter);
  storage::Storage::instance().Stop();
}

int main(int argc, char* argv[]) {
  std::cout << "Storage unit test !!!" << std::endl;
  storage::Storage::instance().Start();
  test1();
  test2();
  //test3();
  return 0;
}

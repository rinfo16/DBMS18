#include "storage/storage_service.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void test1() {
  storage::Storage::instance().Start();
  bool ret = storage::Storage::instance().CreateRelation(1);
  ret = storage::Storage::instance().CreateRelation(2);
  ret = storage::Storage::instance().CreateRelation(3);
  storage::Storage::instance().FlushAll();
  storage::Storage::instance().Stop();
}

void test2() {
  storage::Storage::instance().Start();

  // write to relation 1
  storage::WriteBatchInterface *write_batch = storage::Storage::instance().NewWriteBatch(1);

  char buff[64];
  for (int i = 0; i < 50000; i++) {
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "TUPLE_%d", i);
    Tuple tuple((const uint8_t*)buff, sizeof(buff));
    write_batch->Put(&tuple);
  }
  storage::Storage::instance().DeleteIOObject(write_batch);

  // write to relation 2
  write_batch = storage::Storage::instance().NewWriteBatch(2);

  for (int i = 0; i < 50000; i++) {
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "TUPLE_%d", i);
    Tuple tuple((uint8_t*)buff, sizeof(buff));
    write_batch->Put(&tuple);
  }
  storage::Storage::instance().DeleteIOObject(write_batch);

  storage::Storage::instance().FlushAll();
  storage::Storage::instance().Stop();
}

void test3() {
  storage::Storage::instance().Start();
  storage::IteratorInterface *iter = storage::Storage::instance().NewIterator(1);

  uint32_t length = 0;
  iter->SeekToFirst();
  while (iter->GetStatus() != storage::kStatusEOF) {
    Tuple tuple;
    iter->Get(&tuple);
    std::string str((const char *) tuple.Data(), tuple.Size());
    std::cout << str << std::endl;
    free((void*)tuple.Data());
    iter->Next();
  }
  storage::Storage::instance().DeleteIOObject(iter);
  storage::Storage::instance().Stop();
}

int main(int argc, char* argv[]) {
  std::cout << "Storage unit test !!!" << std::endl;
  storage::Storage::instance().InitDB();
  storage::Storage::instance().Start();
  test1();
  test2();
  test3();
  return 0;
}

#include "storage/storage_service.h"
#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"

class StorageServiceTest : public testing::Test {
 protected:
  virtual void SetUp() {
  }
};

TEST_F(StorageServiceTest, handler_create) {
  storage::Storage::instance().Start();
  storage::RelationHandlerInterface *handler =
      storage::Storage::instance().OpenHandler(1,
                                                      storage::kRelationCreate);
  EXPECT_NE((storage::RelationHandlerInterface * )0, handler);
  EXPECT_EQ(true, handler->Create());
  storage::Storage::instance().CloseHandler(handler);

  handler = storage::Storage::instance().OpenHandler(
      2, storage::kRelationCreate);
  EXPECT_NE((storage::RelationHandlerInterface * )0, handler);
  EXPECT_EQ(true, handler->Create());
  storage::Storage::instance().CloseHandler(handler);
  storage::Storage::instance().FlushAll();
  storage::Storage::instance().Stop();
}

TEST_F(StorageServiceTest, handler_write) {
  storage::Storage::instance().Start();
  storage::RelationHandlerInterface *handler =
      storage::Storage::instance().OpenHandler(1,
                                                      storage::kRelationWrite);
  EXPECT_NE((storage::RelationHandlerInterface*)NULL, handler);

  char buff[64];
  for (int i = 0; i < 50000; i++) {
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "TUPLE_%d", i);
    EXPECT_EQ(true, handler->Put(buff, sizeof(buff)));
  }
  storage::Storage::instance().CloseHandler(handler);
  storage::Storage::instance().FlushAll();
  storage::Storage::instance().Stop();
}

TEST_F(StorageServiceTest, storage1) {
  storage::Storage::instance().Start();
  storage::RelationHandlerInterface *handler =
      storage::Storage::instance().OpenHandler(1,
                                                      storage::kRelationRead);
  EXPECT_NE((storage::RelationHandlerInterface*)NULL, handler);
  uint32_t length = 0;
  const void *tuple = handler->GetFirst(&length);
  while (tuple != NULL) {
    std::string str((const char *) tuple, length);
    std::cout << str << std::endl;
    tuple = handler->GetNext(&length);
  }
  storage::Storage::instance().Stop();
}

int main(int argc, char* argv[]) {
  std::cout << "Storage unit test !!!" << std::endl;
  storage::Storage::instance().InitDB();
  storage::Storage::instance().Start();
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return 0;
}

#include "storage/storage_service.h"
#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"


class StorageServiceTest : public testing::Test {
 protected:
  virtual void SetUp() {
    storage::StorageService::instance().InitDB();
    storage::StorageService::instance().Start();
  }
};

TEST_F(StorageServiceTest, handler_create) {
  storage::RelationHandlerInterface *handler = storage::StorageService::instance().OpenHandler(1, storage::kRelationCreate);
  EXPECT_NE((storage::RelationHandlerInterface *)0, handler);
  EXPECT_EQ(true, handler->Create());
  storage::StorageService::instance().CloseHandler(handler);

  handler = storage::StorageService::instance().OpenHandler(2, storage::kRelationCreate);
  EXPECT_NE((storage::RelationHandlerInterface *)0, handler);
  EXPECT_EQ(true, handler->Create());
  storage::StorageService::instance().CloseHandler(handler);
}

TEST_F(StorageServiceTest, handler_write) {
  storage::RelationHandlerInterface *handler = storage::StorageService::instance().OpenHandler(1, storage::kRelationWrite);
  EXPECT_NE((storage::RelationHandlerInterface*)NULL, handler);

  char buff[64];
  for (int i = 0; i < 50000; i ++)
  {
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "TUPLE_%d", i);
    EXPECT_EQ(true, handler->Put(buff, sizeof(buff)));
  }
  storage::StorageService::instance().CloseHandler(handler);
  storage::StorageService::instance().FlushAll();
  storage::StorageService::instance().Stop();
}

TEST_F(StorageServiceTest, storage1) {
  storage::RelationHandlerInterface *handler = storage::StorageService::instance().OpenHandler(1, storage::kRelationRead);
  EXPECT_NE((storage::RelationHandlerInterface*)NULL, handler);
  uint32_t length = 0;
  void *tuple = handler->GetFirst(&length);
  while (tuple != NULL)
  {
    std::string str((const char *)tuple, length);
    std::cout << str << std::endl;
    tuple = handler->GetNext(&length);
  }
}

int main(int argc, char* argv[]) {
  std::cout << "Storage unit test !!!" << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

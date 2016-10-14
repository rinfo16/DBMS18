#include "storage/storage_service.h"
#include <iostream>
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
  EXPECT_NE(true, handler->Create());
  storage::StorageService::instance().CloseHandler(handler);

  handler = storage::StorageService::instance().OpenHandler(2, storage::kRelationCreate);
  EXPECT_NE((storage::RelationHandlerInterface *)0, handler);
  EXPECT_NE(true, handler->Create());
  storage::StorageService::instance().CloseHandler(handler);

  storage::StorageService::instance().FlushAll();
}

TEST_F(StorageServiceTest, storage1) {

}

int main(int argc, char* argv[]) {
  std::cout << "Storage unit test !!!" << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include "common/relation.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "gtest/gtest.h"

class RelationTest : public testing::Test {
 protected:
  virtual void SetUp() {
  }
};

TEST_F(RelationTest, TestJSON) {
  Relation rel1;

  rel1.SetName("TABLE1");

  PageID pageid;
  pageid.pageno_ = 2;
  pageid.fileno_ = 1;
  rel1.SetFirstDataPageID(pageid);
  rel1.SetLastDataPageID(pageid);

  pageid.pageno_ = 1;
  pageid.fileno_ = 1;
  rel1.SetFirstExtentPageID(pageid);
  rel1.SetLastExtentPageID(pageid);

  rel1.SetID(1);

  Attribute attribute;

  for (int i = 0; i < 10; i++) {
    std::stringstream ssm;
    ssm << "ATTRIBUTE_" << i;
    attribute.SetName(ssm.str());
    attribute.SetID(i);
    attribute.SetMaxLength(10);
    attribute.SetDataType(kDTVarchar);
    attribute.SetNull(true);
    attribute.SetRelationID(1);
    attribute.SetVariable(true);
    rel1.AddAttribute(attribute);
  }
  std::string json = rel1.ToJSON();

  std::cout << json << std::endl;
  std::cout << "JSON STRING LENGTH : " << json.size() << std::endl;

  Relation rel2;
  rel2.InitByJSON(json);
  EXPECT_EQ(rel1.GetName(), rel2.GetName());
  EXPECT_EQ(rel1.GetID(), rel2.GetID());
  EXPECT_EQ(rel1.GetFirstExtentPageID(), rel2.GetFirstExtentPageID());
  EXPECT_EQ(rel1.GetLastExtentPageID(), rel2.GetLastExtentPageID());
  EXPECT_EQ(rel1.GetFirstDataPageID(), rel2.GetFirstDataPageID());
  EXPECT_EQ(rel1.GetLastDataPageID(), rel2.GetLastDataPageID());
  EXPECT_EQ(rel1.GetAttributeCount(), rel2.GetAttributeCount());
  Attribute attr1, attr2;
  attr1 = rel1.GetAttribute(1);
  attr2 = rel2.GetAttribute(1);
  EXPECT_EQ(attr1.GetID(), attr2.GetID());
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return 0;
}

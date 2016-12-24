#include "common/relation.h"
#include <sstream>

boost::property_tree::ptree PageIDToPropertyTree(PageID pageid) {
  boost::property_tree::ptree tree;
  tree.put(STR_FILE_NO, pageid.fileno_);
  tree.put(STR_PAGE_NO, pageid.pageno_);

  return tree;
}

PageID PropertyTreeToPageID(boost::property_tree::ptree tree) {
  PageID pageid;
  std::stringstream ssm;
  ssm << tree.get<fileno_t>(STR_FILE_NO);
  ssm >> pageid.fileno_;
  ssm.clear();
  ssm << tree.get<pageno_t>(STR_PAGE_NO);
  ssm >> pageid.pageno_;

  return pageid;
}

void Relation::FromTuple(const Tuple & tuple, uint32_t size) {
  Slot *slot = tuple->GetSlot(0);
  relation_id_ = tuple->GetInteger(slot->offset_);

  slot = tuple->GetSlot(1);
  name_ = std::string(tuple->GetValue(slot->offset_), slot->length_);
}

void Relation::ToTuple(Tuple *t, uint32_t *length) const {
  Tuple tuple = memory::CreateTuple(512);

  uint32_t offset = 0;
  offset += sizeof(Slot) * 2;

// select id, name from sys_relation;

// relation id
  Slot *slot = tuple->GetSlot(0);
  tuple->SetValue(offset, &relation_id_, sizeof(relation_id_));
  slot->offset_ = offset;
  slot->length_ = sizeof(relation_id_);
  offset += sizeof(relation_id_);

// name
  slot = tuple->GetSlot(1);
  tuple->SetValue(offset, name_.c_str(), name_.size());
  slot->offset_ = offset;
  slot->length_ = name_.size();
  offset += name_.size();

  *t = tuple;
  *length + offset;
}

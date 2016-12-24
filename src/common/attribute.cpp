#include "common/attribute.h"

void Attribute::FromTuple(const Tuple & tuple, uint32_t size) {
  // attribute id
  Slot *slot = tuple->GetSlot(0);
  attribute_id_ = std::string(tuple->GetValue(slot->offset_), slot->length_);

  // relation id
  slot = tuple->GetSlot(1);
  relation_id_ = std::string(tuple->GetValue(slot->offset_), slot->length_);


  // attribute name
  slot = tuple->GetSlot(2);
  name_ = std::string(tuple->GetValue(slot->offset_), slot->length_);


  // data type
  slot = tuple->GetSlot(3);
  data_type_ = (DataType) tuple->GetInteger(slot->offset_);

  // data size
  slot = tuple->GetSlot(4);
  max_length_ = tuple->GetInteger(slot->offset_);
}

void Attribute::ToTuple(Tuple *t, uint32_t *length) const {
  Tuple tuple =  memory::CreateTuple(1024);

  uint32_t offset = sizeof(Slot) * 4;
  tuple->SetValue(offset, attribute_id_.c_str(), attribute_id_.size());
  Slot *slot = tuple->GetSlot(0);
  slot->offset_ = offset;
  slot->length_ = attribute_id_.size();
  offset += attribute_id_.size();

  // rel_id
  tuple->SetValue(offset, relation_id_.c_str(), relation_id_.length());
  slot = tuple->GetSlot(1);
  slot->offset_ = offset;
  slot->length_ = relation_id_.length();
  offset += relation_id_.length();

  // name
  tuple->SetValue(offset, name_.c_str(), name_.length());
  slot = tuple->GetSlot(2);
  slot->offset_ = offset;
  slot->length_ = name_.length();
  offset += name_.length();

  // type
  uint64_t type = data_type_;
  tuple->SetValue(offset, &type, sizeof(type));
  slot = tuple->GetSlot(3);
  slot->offset_ = offset;
  slot->length_ = sizeof(type);
  offset += sizeof(type);

  // size
  uint64_t size = max_length_;
  tuple->SetValue(offset, &size, sizeof(size));
  slot = tuple->GetSlot(4);
  slot->offset_ = offset;
  slot->length_ = size;
  offset += size;

  *t = tuple;
  *length = offset;
}


#ifndef DB_INDEX_H__
#define DB_INDEX_H__

#include "common/tuple.h"

class DBIndex {
 public:
  DBIndex()
      : id_(0),
        relation_id_(0),
        attribute_id_(0) {
  }
  const std::string & GetName() const {
    return name_;
  }
  void SetName(const std::string & name) {
    name_ = name;
  }
  uint32_t GetRelationID() const {
    return relation_id_;
  }
  void SetRelationID(uint32_t id) {
    relation_id_ = id;
  }
  uint32_t GetAttributeID() const {
    return attribute_id_;
  }
  void SetAttribvuteID(uint32_t id) {
    attribute_id_ = id;
  }

  void SetSegmentID(PageID id) {
    segment_id_ = id;
  }

  PageID GetSegmentID() const {
    return segment_id_;
  }

  Tuple ToTuple() {
    Tuple tuple = memory::CreateTuple(sizeof(int64_t) * 3 + name_.size());
    uint32_t offset = sizeof(Slot) * 3;
    Slot *slot = tuple->GetSlot(0);

    int64_t id = id_;
    tuple->SetValue(offset, &id, sizeof(id));
    offset += sizeof(id);
    slot->length_ = sizeof(id);
    slot->offset_ = offset;

    tuple->GetSlot(1);
    int64_t rel_id = relation_id_;
    tuple->SetValue(offset, &rel_id, sizeof(rel_id));
    offset += sizeof(rel_id);
    slot->length_ = sizeof(rel_id);
    slot->offset_ = offset;

    tuple->GetSlot(2);
    int64_t attr_id = attribute_id_;
    tuple->SetValue(offset, &attr_id, sizeof(attr_id));
    offset += sizeof(attr_id);
    slot->length_ = sizeof(attr_id);
    slot->offset_ = offset;

    tuple->GetSlot(3);
    tuple->SetValue(offset, name_.c_str(), name_.size());
    offset += name_.size();
    slot->length_ = name_.size();
    slot->offset_ = offset;

    tuple->GetSlot(4);
    tuple->SetValue(offset, &segment_id_, sizeof(segment_id_));
    offset += sizeof(segment_id_);
    slot->length_ = sizeof(segment_id_);
    slot->offset_ = offset;

    return tuple;
  }

  void InitFromTuple(const Tuple tuple) {
    Slot *slot = tuple->GetSlot(0);
    id_ = tuple->GetInteger(slot->offset_);

    slot = tuple->GetSlot(1);
    relation_id_ = tuple->GetInteger(slot->offset_);

    slot = tuple->GetSlot(2);
    attribute_id_ = tuple->GetInteger(slot->offset_);

    slot = tuple->GetSlot(3);
    name_ = std::string(tuple->GetValue(slot->offset_), slot->length_);

    slot = tuple->GetSlot(4);
    memcpy(&segment_id_, tuple->GetValue(slot->offset_), 8);
  }

 private:
  uint32_t id_;
  uint32_t relation_id_;
  uint32_t attribute_id_;
  std::string name_;
  PageID segment_id_;
};

#endif // DB_INDEX_H__

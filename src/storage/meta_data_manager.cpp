#include "meta_data_manager.h"
#include <algorithm>
#include <sstream>
#include "common/config.h"
#include "page_operation.h"
#include "iterator.h"

#define META_DATA_FILE "sys.catalog"

namespace storage {

MetaDataManager::MetaDataManager(BufferManager *buffer_manager,
                                 SpaceManager *space_manager) {
  data_directory_ = config::Setting::instance().data_directory_;
  buffer_manager_ = buffer_manager;
  space_manager_ = space_manager;
}

MetaDataManager::~MetaDataManager() {

}

bool MetaDataManager::InsertRelation(Relation *rel) {
  auto ret1 = name_rel_map_.insert(std::make_pair(rel->GetName(), rel));
  if (!ret1.second) {
    return false;
  }
  auto ret2 = id_rel_map_.insert(std::make_pair(rel->GetID(), rel));
  if (!ret2.second) {
    return false;
  }
  all_relations_.push_back(rel);
  return true;
}

void MetaDataManager::AddRelation(Relation *rel) {
  if (rel->GetName().find("sys_") != 0) {  // not a system table
    bool ok = InsertRelation(rel);
    if (!ok)
      return;
  }

  Tuple tuple = memory::CreateTuple(512);

  uint32_t offset = 0;
  offset += sizeof(Slot) * 4;

  // id
  uint64_t relation_id = rel->GetID();
  tuple->SetValue(offset, &relation_id, sizeof(relation_id));
  offset += sizeof(relation_id);

  // name
  std::string rel_name = rel->GetName();
  tuple->SetValue(offset, rel_name.c_str(), rel_name.size());
  offset += rel_name.size();

  // is_rel
  uint64_t is_rel = 1;
  tuple->SetValue(offset, &is_rel, sizeof(is_rel));
  offset += sizeof(is_rel);

  // seg_id
  uint64_t seg_id = *((uint64_t*) &rel->GetSegmentID());
  tuple->SetValue(offset, &seg_id, sizeof(seg_id));
  offset += sizeof(seg_id);

  PageID segid_class;
  segid_class.fileno_ = 0;
  segid_class.pageno_ = 1;

  space_manager_->WriteTuple(segid_class, tuple.get(), offset, false);

  for (int i = 0; i < rel->GetAttributeCount(); i++) {
    uint32_t offset = 0;
    offset += sizeof(Slot) * 5;

    const Attribute &attr = rel->GetAttribute(i);

    //id
    uint64_t attr_id = attr.GetID();
    tuple->SetValue(offset, &attr_id, sizeof(relation_id));
    offset += sizeof(attr_id);

    // name
    std::string name = attr.GetName();
    tuple->SetValue(offset, name.c_str(), name.length());
    offset += sizeof(name.length());

    // rel_id
    uint64_t rel_id = attr.GetRelationID();
    tuple->SetValue(offset, &rel_id, sizeof(rel_id));
    offset += sizeof(rel_id);

    // type
    uint64_t type = attr.GetDataType();
    tuple->SetValue(offset, &type, sizeof(type));
    offset += sizeof(type);

    // size
    uint64_t size = attr.GetMaxLength();
    tuple->SetValue(offset, &size, sizeof(size));
    offset += sizeof(size);

    segid_class.fileno_ = 0;
    segid_class.pageno_ = 2;

    space_manager_->WriteTuple(segid_class, tuple.get(), offset, false);
  }

}

Relation* MetaDataManager::GetRelationByName(const std::string &name) {
  auto ret = name_rel_map_.find(name);
  if (ret == name_rel_map_.end()) {
    return NULL;
  } else {
    return ret->second;
  }
}

Relation* MetaDataManager::GetRelationByID(relationid_t id) {
  auto ret = id_rel_map_.find(id);
  if (ret == id_rel_map_.end()) {
    return NULL;
  } else {
    return ret->second;
  }
}

bool MetaDataManager::RemoveRelationByName(const std::string & name) {
  auto ret = name_rel_map_.find(name);
  if (ret == name_rel_map_.end()) {
    return false;
  }
  Relation *rel = ret->second;
  name_rel_map_.erase(ret);
  id_rel_map_.erase(rel->GetID());
  std::remove(all_relations_.begin(), all_relations_.end(), rel);
  delete rel;
  return true;
}

bool MetaDataManager::RemoveRelationByID(relationid_t id) {
  auto ret = id_rel_map_.find(id);
  if (ret == id_rel_map_.end()) {
    return false;
  }
  Relation *rel = ret->second;
  id_rel_map_.erase(ret);
  name_rel_map_.erase(rel->GetName());
  std::remove(all_relations_.begin(), all_relations_.end(), rel);
  delete rel;
  return true;
}

bool MetaDataManager::Start() {
  PageID seg_file_hdr_pageid;
  seg_file_hdr_pageid.fileno_ = 0;
  seg_file_hdr_pageid.pageno_ = 0;
  Page *seg_file_hdr_page = buffer_manager_->FixPage(seg_file_hdr_pageid);
  SegFileHeader *seg_file_hdr = ToSegFileHeader(seg_file_hdr_page);
  if (seg_file_hdr->data_file_count_ == 0) {
    CreateSysTable();
  }
  buffer_manager_->UnfixPage(seg_file_hdr_page);

  ReadSysTable();
  BOOST_LOG_TRIVIAL(info)<< "meta data manager start.";
  return true;
}

void MetaDataManager::Stop() {
  for (size_t i = 0; i < all_relations_.size(); i++) {
    delete all_relations_[i];
  }
  all_relations_.clear();
  id_rel_map_.clear();
  name_rel_map_.clear();
  BOOST_LOG_TRIVIAL(info)<< "meta data manager stop.";
}

void MetaDataManager::ReadSysTable() {
  PageID seg_class;
  seg_class.fileno_ = 0;
  seg_class.pageno_ = 1;
  Iterator iter_class(seg_class, buffer_manager_);
  iter_class.SeekToFirst();
  uint32_t length;
  while (true) {
    const void *p = iter_class.Get(&length);
    if (p) {
      Tuple tuple = memory::CreateTuple(length);
      memcpy(tuple->Data(), p, length);

      Relation *rel = new Relation();
      Slot *slot = tuple->GetSlot(0);
      rel->SetID(tuple->GetInteger(slot->offset_));

      slot = tuple->GetSlot(1);
      rel->SetName(
          std::string((const char*) tuple->GetValue(slot->offset_),
                      slot->length_));

      slot = tuple->GetSlot(2);
      PageID id = *(const PageID *) tuple->GetValue(slot->offset_);
      rel->SetSegmentID(id);
      InsertRelation(rel);
      iter_class.Next();
    } else {
      break;
    }
  }

  Iterator iter_attr(seg_class, buffer_manager_);
  iter_attr.SeekToFirst();
  while (true) {
    const void *p = iter_attr.Get(&length);
    if (p) {
      Tuple tuple = memory::CreateTuple(length);
      memcpy(tuple->Data(), p, length);

      Attribute attr;
      Slot *slot = tuple->GetSlot(0);
      attr.SetID(tuple->GetInteger(slot->offset_));

      slot = tuple->GetSlot(1);
      attr.SetName(
          std::string((const char*) tuple->GetValue(slot->offset_),
                      slot->length_));

      slot = tuple->GetSlot(2);
      attr.SetNull(tuple->GetInteger(slot->offset_));

      slot = tuple->GetSlot(3);
      attr.SetDataType((DataType) tuple->GetInteger(slot->offset_));

      slot = tuple->GetSlot(4);
      attr.SetMaxLength(tuple->GetInteger(slot->offset_));

      Relation *rel = GetRelationByID(attr.GetRelationID());
      assert(rel);
      rel->AddAttribute(attr);
      iter_attr.Next();
    } else {
      break;
    }
  }
}

void MetaDataManager::CreateSysTable() {
  PageID id1, id2;
  // sys_class
  bool ok = space_manager_->CreateSegment(&id1);
  // sys_attribute
  ok = space_manager_->CreateSegment(&id2);

  Relation* rel = new Relation();
  rel->SetName("sys_class");
  rel->SetID(0);
  rel->SetSegmentID(id1);

  Attribute attribute;
  attribute.SetName("id");
  attribute.SetID(0);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);

  rel->AddAttribute(attribute);

  attribute.SetName("name");
  attribute.SetID(1);
  attribute.SetDataType(kDTVarchar);
  attribute.SetNull(false);
  attribute.SetMaxLength(512);
  attribute.SetVariable(false);
  rel->AddAttribute(attribute);

  attribute.SetName("segment_id");
  attribute.SetID(2);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel->AddAttribute(attribute);
  this->AddRelation(rel);

  rel = new Relation();
  rel->SetName("sys_attribute");
  rel->SetID(1);
  rel->SetSegmentID(id2);

  attribute.SetName("id");
  attribute.SetID(0);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel->AddAttribute(attribute);

  attribute.SetName("name");
  attribute.SetID(1);
  attribute.SetDataType(kDTVarchar);
  attribute.SetNull(false);
  attribute.SetMaxLength(512);
  attribute.SetVariable(false);
  rel->AddAttribute(attribute);

  attribute.SetName("rel_id");
  attribute.SetID(2);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel->AddAttribute(attribute);

  attribute.SetName("type");
  attribute.SetID(3);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel->AddAttribute(attribute);

  attribute.SetName("size");
  attribute.SetID(4);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel->AddAttribute(attribute);

  this->AddRelation(rel);

}

}  // namespace storage

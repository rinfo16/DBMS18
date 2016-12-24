#include <stdlib.h>
#include <sstream>
#include <boost/filesystem.hpp>
#include "leveldb/db.h"
#include "leveldb/comparator.h"
#include "iterator_handler_impl.h"
#include "level_store.h"
#include "write_handler_impl.h"
#include "uuid.h"

#define SYS_RELATION 0
#define SYS_ATTRIBUTE 1
#define SYS_INDEX     2

namespace storage {

template<class T>
std::string ToString(const T & v) {
  std::stringstream ssm;
  ssm << v;
  return ssm.str();
}

LevelStore::LevelStore() {
  db_attribute_ = NULL;
  db_class_ = NULL;
}

LevelStore::~LevelStore() {
  delete db_attribute_;
  delete db_class_;
}

bool LevelStore::InitDB() {
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, GetDBFilePath(SYS_RELATION), &db_class_);
  if (!status.ok())
    return false;

  status = leveldb::DB::Open(options, GetDBFilePath(SYS_ATTRIBUTE), &db_attribute_);
  if (!status.ok())
    return false;

  assert(db_class_);
  assert(db_attribute_);

  CreateSystemTable();

  delete db_class_;
  delete db_attribute_;

  db_attribute_ = NULL;
  db_class_ = NULL;
  return true;
}
bool LevelStore::Start() {
  //if (! boost::filesystem::exists(GetDBFilePath(SYS_RELATION)))
  {
    InitDB();
  }

  leveldb::Options options;
  leveldb::Status status = leveldb::DB::Open(options, GetDBFilePath(SYS_RELATION), &db_class_);
  if (!status.ok())
    return false;

  status = leveldb::DB::Open(options, GetDBFilePath(SYS_ATTRIBUTE), &db_attribute_);
  if (!status.ok())
    return false;

  assert(db_class_);
  assert(db_attribute_);

  ReadSystemTable();
  return true;
}

void LevelStore::Stop() {

  delete db_attribute_;
  delete db_class_;
  db_attribute_ = NULL;
  db_class_ = NULL;
}

bool LevelStore::CreateRelation(const TableSchema & schema) {
  std::auto_ptr<Relation> rel(new Relation());
  rel->SetName(schema.name_);
  rel->SetID(GenRelationID());

  std::vector<std::pair<ColumnSchema, int> > columns;
  for (int i = 0; i < schema.column_list_.size(); i++) {
    columns.push_back(std::make_pair(schema.column_list_[i], i));
  }

  for (size_t i = 0; i < columns.size(); i++) {
    ColumnSchema & col = columns[i].first;
    Attribute attribute;
    attribute.SetName(col.name_);
    attribute.SetID(columns[i].second);
    attribute.SetDataType(col.data_type_);
    attribute.SetNull(col.is_null_);
    uint32_t size = 0;
    bool variable = false;
    switch (col.data_type_) {
      case kDTInteger:
        size = sizeof(int64_t);
        break;
      case kDTFloat:
        size = sizeof(int64_t);
        break;
      case kDTVarchar:
        variable = true;
        size = col.length_;
        break;
      case kDTDate:
        size = sizeof(int64_t);
        break;
      default:
        assert(false);
        break;
    }
    attribute.SetMaxLength(size);
    attribute.SetVariable(variable);

    rel->AddAttribute(attribute);
  }

  WriteRelMeta(rel);
  InsertRelation(rel);
  return true;
}
bool LevelStore::DropRelation(const std::string & rel_name) {
  return true;
}

bool LevelStore::CreateIndex(const IndexSchema & index_schema) {
  return true;
}

IteratorHandler * LevelStore::NewIterator(relationid_t relid) {
  return new IteratorHandlerImpl(GetDBFilePath(relid));
}

WriteHandler * LevelStore::NewWriteHandler(relationid_t relid) {
  return new WriteHandlerImpl(GetDBFilePath(relid));
}
void LevelStore::DeleteIOObject(IOHandler* io_object) {
  delete io_object;
}

void LevelStore::WriteRelMeta(std::auto_ptr<Relation> r) {
  // auto_ptr r is release now ..
  leveldb::WriteOptions write_options;
  write_options.sync = true;

  Tuple tuple;
  uint32_t size;

  r->ToTuple(&tuple, &size);
  leveldb::Slice key(GenUUID());
  leveldb::Slice value(tuple->Ptr(), size);
  assert(key.size() == 16);
  db_class_->Put(write_options, key, value);
  for (int i = 0; i < r->GetAttributeCount(); i++) {
    const Attribute & attr = r->GetAttribute(i);
    Tuple tuple;
    uint32_t size;
    attr.ToTuple(&tuple, &size);
    leveldb::Slice key(GenUUID());
    leveldb::Slice value(tuple->Ptr(), size);
    assert(key.size() == 32);
    db_attribute_->Put(write_options, key, value);
  }
}

bool LevelStore::InsertRelation(std::auto_ptr<Relation> r) {
  auto ret1 = name_rel_map_.insert(std::make_pair(r->GetName(), r.get()));
  if (!ret1.second) {
    return false;
  }

  all_relations_.push_back(r);
  return true;
}

bool LevelStore::CreateSystemTable() {
  std::auto_ptr<Relation> rel_class(new Relation());
  rel_class->SetName("sys_class");
  rel_class->SetID(SYS_RELATION);

  Attribute attribute;
  attribute.SetName("id");
  attribute.SetID(0);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel_class->AddAttribute(attribute);

  attribute.SetName("name");
  attribute.SetID(1);
  attribute.SetDataType(kDTVarchar);
  attribute.SetNull(false);
  attribute.SetMaxLength(512);
  attribute.SetVariable(false);
  rel_class->AddAttribute(attribute);

  this->WriteRelMeta(rel_class);

  std::auto_ptr<Relation> rel_attr(new Relation());
  rel_attr->SetName("sys_attribute");
  rel_attr->SetID(SYS_ATTRIBUTE);

  attribute.SetName("id");
  attribute.SetID(0);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel_attr->AddAttribute(attribute);

  attribute.SetName("rel_id");
  attribute.SetID(1);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel_attr->AddAttribute(attribute);

  attribute.SetName("name");
  attribute.SetID(2);
  attribute.SetDataType(kDTVarchar);
  attribute.SetNull(false);
  attribute.SetMaxLength(512);
  attribute.SetVariable(false);
  rel_attr->AddAttribute(attribute);

  attribute.SetName("type");
  attribute.SetID(3);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel_attr->AddAttribute(attribute);

  attribute.SetName("size");
  attribute.SetID(4);
  attribute.SetDataType(kDTInteger);
  attribute.SetNull(false);
  attribute.SetMaxLength(8);
  attribute.SetVariable(false);
  rel_attr->AddAttribute(attribute);

  this->WriteRelMeta(rel_attr);

  return true;

}
bool LevelStore::ReadSystemTable() {

  leveldb::Iterator *it_rel = db_class_->NewIterator(leveldb::ReadOptions());
  for (it_rel->SeekToFirst(); it_rel->Valid(); it_rel->Next()) {
    Tuple tuple = memory::CreateTuple(it_rel->value().size());
    memcpy(tuple.get(), it_rel->value().data(), it_rel->value().size());
    std::auto_ptr<Relation> r(new Relation());
    r->FromTuple(tuple, it_rel->value().size());
    InsertRelation(r);
  }
  assert(it_rel->status().ok());
  delete it_rel;

  leveldb::Iterator *it_attr = db_attribute_->NewIterator(
      leveldb::ReadOptions());
  for (it_attr->SeekToFirst(); it_attr->Valid(); it_attr->Next()) {
    Tuple tuple = memory::CreateTuple(it_attr->value().size());
    memcpy(tuple.get(), it_attr->value().data(), it_attr->value().size());
    Attribute attribute;
    attribute.FromTuple(tuple, it_attr->value().size());
    auto iter = name_rel_map_.find(attribute.GetName());
    assert(iter != name_rel_map_.end());
    Relation *rel = iter->second;
    rel->AddAttribute(attribute);
  }
  assert(it_attr->status().ok());
  delete it_attr;

  return true;
}

Relation *LevelStore::GetRelation(const std::string &rel_name) {
  auto iter = name_rel_map_.find(rel_name);
  if (iter == name_rel_map_.end())
    return NULL;
  else
    return iter->second;
}

std::string LevelStore::GetDBFilePath(relationid_t id) {
  std::string path = path_ + "/" + ToString(id) + ".db";
  return path;
}

relationid_t LevelStore::GenRelationID() {
  while (true) {
    relationid_t id = rand();
    if (id_rel_map_.find(id) == id_rel_map_.end())
      return id;
  }
}

}

// namespace storage

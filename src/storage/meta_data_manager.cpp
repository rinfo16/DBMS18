#include "meta_data_manager.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include "common/config.h"
#include "page_operation.h"

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

void MetaDataManager::AddRelation(Relation *rel) {
  auto ret1 = name_rel_map_.insert(std::make_pair(rel->GetName(), rel));
  if (!ret1.second) {
    return;
  }
  auto ret2 = id_rel_map_.insert(std::make_pair(rel->GetID(), rel));
  if (!ret2.second) {
    return;
  }
  all_relations_.push_back(rel);
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
  for (uint32_t i = 1; i <= seg_file_hdr->page_count_; i++) {
    PageID pageid;
    pageid.fileno_ = 0;
    pageid.pageno_ = i;
    Page *segment_desc_page = buffer_manager_->FixPage(pageid);

    SegmentHeader *seg_hdr = ToSegmentHeader(segment_desc_page);
    std::string json(seg_hdr->schema_, seg_hdr->schema_data_length_);

    Relation *rel = new Relation();
    rel->InitByJSON(json);
    AddRelation(rel);

    buffer_manager_->UnfixPage(segment_desc_page);
  }
  buffer_manager_->UnfixPage(seg_file_hdr_page);
  return true;
}

void MetaDataManager::Stop() {
  for (size_t i = 0; i < all_relations_.size(); i++) {
    Relation *rel = all_relations_[i];

    // write to page ...
    PageID pageid;
    pageid.pageno_ = rel->GetID();
    pageid.fileno_ = SEGMENT_DESCRIPT_FILE_NO;
    Page* page = buffer_manager_->FixPage(pageid);
    SegmentHeader *seg_hdr = ToSegmentHeader(page);
    std::string schema_json = rel->ToJSON();
    seg_hdr->schema_data_length_ = (uint32_t) schema_json.size();
    memcpy(seg_hdr->schema_, schema_json.c_str(), schema_json.size());
    PageGetFrame(page)->SetDirty(true);
    buffer_manager_->UnfixPage(page);

    delete rel;
  }
  all_relations_.clear();
  id_rel_map_.clear();
  name_rel_map_.clear();
}

}  // namespace storage

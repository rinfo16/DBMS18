#ifndef RELATION_H_
#define RELATION_H_

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "common/define.h"
#include "common/page_id.h"
#include "common/attribute.h"
#include "row_desc.h"

boost::property_tree::ptree PageIDToPropertyTree(PageID pageid);
PageID PropertyTreeToPageID(boost::property_tree::ptree tree);

class Relation {
 public:
  Relation()
      : id_(0) {
  }
  ~Relation() {
  }

  void SetName(const std::string & name) {
    name_ = name;
  }
  const std::string & GetName() const {
    return name_;
  }

  void SetSegmentID(const PageID & id) {
    segment_id_ = id;
  }

  const PageID & GetSegmentID() const {
    return segment_id_;
  }

  void SetID(relationid_t id) {
    id_ = id;
  }

  relationid_t GetID() const {
    return id_;
  }

  size_t GetAttributeCount() const {
    return attributes_.size();
  }

  Attribute & GetAttribute(size_t n) {
    return attributes_[n];
  }

  const Attribute & GetAttribute(size_t n) const {
    return attributes_[n];
  }

  void AddAttribute(const Attribute & attribute) {
    attributes_.push_back(attribute);
    attributes_.back().SetRelationName(name_);
    attributes_.back().SetRelationID(id_);
  }

  void SetFirstExtentPageID(PageID pageid) {
    first_extent_pageid_ = pageid;
  }

  PageID GetFirstExtentPageID() const {
    return first_extent_pageid_;
  }

  void SetLastExtentPageID(PageID pageid) {
    last_extent_pageid_ = pageid;
  }
  PageID GetLastExtentPageID() const {
    return last_extent_pageid_;
  }

  void SetFirstDataPageID(PageID pageid) {
    first_data_pageid_ = pageid;
  }
  PageID GetFirstDataPageID() const {
    return first_data_pageid_;
  }

  void SetLastDataPageID(PageID pageid) {
    last_data_pageid_ = pageid;
  }
  PageID GetLastDataPageID() const {
    return last_data_pageid_;
  }

  int32_t GetAttributeIndex(const std::string & attribute_name) const {
    for (int i = 0; i < attributes_.size(); i++) {
      if (attributes_[i].GetName() == attribute_name) {
        return i;
      }
    }
    return -1;
  }

  boost::property_tree::ptree ToPropertyTree() {
    boost::property_tree::ptree relation;
    boost::property_tree::ptree attributes;
    relation.put(STR_NAME, name_);
    relation.put(STR_ID, id_);
    relation.put_child(STR_FIRST_DATA_PAGE,
                       PageIDToPropertyTree(first_data_pageid_));
    relation.put_child(STR_LAST_DATA_PAGE,
                       PageIDToPropertyTree(last_data_pageid_));
    relation.put_child(STR_FIRST_EXTENT,
                       PageIDToPropertyTree(first_extent_pageid_));
    relation.put_child(STR_LAST_EXTENT,
                       PageIDToPropertyTree(last_extent_pageid_));
    for (size_t i = 0; i < attributes_.size(); i++) {
      std::stringstream ssm;
      ssm << i;
      boost::property_tree::ptree attr = attributes_[i].ToPropertyTree();
      attributes.push_back(std::make_pair(ssm.str(), attr));
    }
    relation.put_child(STR_ATTRIBUTE_LIST, attributes);
    return relation;
  }

  void InitByPropertyTree(const boost::property_tree::ptree & tree) {
    name_ = tree.get<std::string>(STR_NAME);
    id_ = tree.get<uint32_t>(STR_ID);

    boost::property_tree::ptree id_tree;
    id_tree = tree.get_child(STR_FIRST_DATA_PAGE);
    first_data_pageid_.fileno_ = id_tree.get<uint32_t>(STR_FILE_NO);
    first_data_pageid_.pageno_ = id_tree.get<uint32_t>(STR_PAGE_NO);

    id_tree = tree.get_child(STR_LAST_DATA_PAGE);
    last_data_pageid_.fileno_ = id_tree.get<uint32_t>(STR_FILE_NO);
    last_data_pageid_.pageno_ = id_tree.get<uint32_t>(STR_PAGE_NO);

    id_tree = tree.get_child(STR_FIRST_EXTENT);
    first_extent_pageid_.fileno_ = id_tree.get<uint32_t>(STR_FILE_NO);
    first_extent_pageid_.pageno_ = id_tree.get<uint32_t>(STR_PAGE_NO);

    id_tree = tree.get_child(STR_LAST_EXTENT);
    last_extent_pageid_.fileno_ = id_tree.get<uint32_t>(STR_FILE_NO);
    last_extent_pageid_.pageno_ = id_tree.get<uint32_t>(STR_PAGE_NO);

    boost::property_tree::ptree attributes_tree;
    attributes_tree = tree.get_child(STR_ATTRIBUTE_LIST);
    for (boost::property_tree::ptree::iterator iter = attributes_tree.begin();
        iter != attributes_tree.end(); ++iter) {
      boost::property_tree::ptree & attr_tree = iter->second;
      Attribute attr;
      attr.InitFromPropertyTree(attr_tree);
      AddAttribute(attr);
    }
  }

  std::string ToJSON() {
    std::stringstream ssm;
    boost::property_tree::ptree relation = ToPropertyTree();
    boost::property_tree::write_json(ssm, relation);
    return ssm.str();
  }

  void InitByJSON(std::string & json) {
    std::stringstream ssm;
    ssm << json;
    boost::property_tree::ptree relation_tree;
    boost::property_tree::read_json(ssm, relation_tree);
    InitByPropertyTree(relation_tree);
  }

  RowDesc ToDesc() const {
    RowDesc desc;
    uint32_t off = 0;
    bool variable = false;
    //desc.mapping_.resize(attributes_.size(), 0);
    for (size_t i = 0; i < attributes_.size(); i++) {
      const Attribute attr = attributes_[i];
      ColumnDesc column_desc;
      column_desc.column_name_ = attr.GetName();
      column_desc.data_type_ = attr.GetDataType();
      column_desc.data_type_size_ = attr.GetMaxLength();
      column_desc.attribute_id_ = attr.GetID();
      column_desc.item_slot_.nth_tuple_ = 0;
      column_desc.item_slot_.nth_item_ = i;
      desc.PushColumnDesc(column_desc);
      //desc.data_type_.push_back(attr.GetDataType());
      //desc.mapping_[attr.GetID()] = i;
      //desc.column_count_ ++;
    }
    return desc;
  }

 private:
  std::string name_;
  relationid_t id_;
  PageID first_data_pageid_;
  PageID last_data_pageid_;
  PageID last_extent_pageid_;
  PageID first_extent_pageid_;
  PageID segment_id_;
  std::vector<Attribute> attributes_;
};


#endif // RELATION_H_

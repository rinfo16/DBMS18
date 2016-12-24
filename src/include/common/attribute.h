#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "common/data_type.h"
#include "common/define.h"
#include "common/tuple.h"


class Attribute {
 public:
  Attribute(const std::string & name,
            std::string & relation_id,
            uint32_t attribute_id,
            DataType data_type,
            uint32_t length,
            bool is_null = false,
            bool is_variable = false)
      : id_(attribute_id),
        relation_id_(relation_id),
        max_length_(length),
        data_type_(data_type),
        is_null_(is_null),
        is_variable_(is_variable),
        attribute_index_(0) {
  }
  Attribute()
      : id_(0),
        relation_id_(0),
        max_length_(0),
        data_type_(kDTInvalid),
        is_null_(false),
        is_variable_(false),
        attribute_index_(0) {
  }

  ~Attribute() {
  }

  void SetName(const std::string & name) {
    name_ = name;
  }
  const std::string & GetName() const {
    return name_;
  }

  void SetID(attributeid_t  id) {
    id_ = id;
  }
  attributeid_t GetID() const {
    return id_;
  }


  void SetRelationID(relationid_t id) {
    relation_id_ = id;
  }
  const std::string & GetRelationID() const {
    return relation_id_;
  }

  void SetDataType(DataType data_type) {
    data_type_ = data_type;
  }
  DataType GetDataType() const {
    return data_type_;
  }

  void SetMaxLength(uint32_t length) {
    max_length_ = length;
  }
  uint32_t GetMaxLength() const {
    return max_length_;
  }

  void SetNull(bool is_null) {
    is_null_ = is_null;
  }
  bool IsNull() const {
    return is_null_;
  }

  void SetVariable(bool is_variable) {
    is_variable_ = is_variable;
  }
  bool IsVariable() const {
    return is_variable_;
  }

  // position in relation attribute list
  void SetAttributeIndex(uint32_t i) {
    attribute_index_ = i;
  }
  // position in relation attribute list
  uint32_t GetAttributeIndex() const {
    return attribute_index_;
  }
  boost::property_tree::ptree ToPropertyTree() const {
    boost::property_tree::ptree attribute;

    attribute.put(STR_NAME, name_);
    attribute.put(STR_ID, id_);
    attribute.put(STR_RELATION_ID, relation_id_);
    attribute.put(STR_MAX_LENGTH, max_length_);
    attribute.put(STR_DATA_TYPE, (uint32_t) data_type_);
    attribute.put(STR_IS_NULL, is_null_);
    attribute.put(STR_IS_VARIABLE, is_variable_);

    return attribute;
  }

  void InitFromPropertyTree(const boost::property_tree::ptree &tree) {
    name_ = tree.get < std::string > (STR_NAME);
    id_ = tree.get < attributeid_t > (STR_ID);
    relation_id_ = tree.get < relationid_t > (STR_RELATION_ID);
    max_length_ = tree.get < uint32_t > (STR_MAX_LENGTH);
    data_type_ = (DataType) tree.get < uint32_t > (STR_DATA_TYPE);
    is_null_ = tree.get<bool>(STR_IS_NULL);
    is_variable_ = tree.get<bool>(STR_IS_VARIABLE);
  }
  void FromTuple(const Tuple & tuple, uint32_t size);
  void ToTuple(Tuple *t, uint32_t *size) const;
 private:
  std::string name_;
  uint32_t attribute_index_;
  attributeid_t id_;
  std::string relation_id_;
  uint32_t max_length_;
  DataType data_type_;
  std::string attribute_id_;
  bool is_null_;
  bool is_variable_;
};

#endif // ATTRIBUTE_H_

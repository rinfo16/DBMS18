#ifndef DB_INDEX_H__
#define DB_INDEX_H__

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
 private:
  uint32_t id_;
  uint32_t relation_id_;
  uint32_t attribute_id_;
  std::string name_;
};

#endif // DB_INDEX_H__

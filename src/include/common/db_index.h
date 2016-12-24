#ifndef INDEX_H__
#define INDEX_H__

#include "common/define.h"

class Index {
 public:
  void SetName(const std::string & name);
 private:
  std::string & name_;
  relationid_t relation_id_;
};

#endif // INDEX_H__

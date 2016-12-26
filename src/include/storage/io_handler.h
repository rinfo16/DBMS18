#ifndef IO_OBJECT_INTERFACE_H_
#define IO_OBJECT_INTERFACE_H_

#include <boost/utility.hpp>

namespace storage {

class IOHandler : public boost::noncopyable {
 public:
  IOHandler() { relid_ = 0; reference_ = 0; }
  IOHandler(relationid_t relid) { relid_ = relid; reference_ = 0; }
  virtual ~IOHandler() {};
  void AddRef(){reference_++;};
  void SubRef(){reference_--;};
  int Ref() {return reference_;};
  void SetRelationID(relationid_t relid) { relid_ = relid; }
  relationid_t GetRelationID() { return relid_; };
 private:
  relationid_t relid_;
  int reference_;
};

}  // namespace storage

#endif // IO_OBJECT_INTERFACE_H_

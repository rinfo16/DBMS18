#ifndef ITERATOR_H_
#define ITERATOR_H_

#include "storage_define.h"


#include "storage/iterator_interface.h"
#include "relation_handler.h"
namespace storage {

class Iterator : public IteratorInterface {
 public:
  Iterator(RelationHandlerInterface *rel_handler);
  ~Iterator();
  virtual bool Get(Tuple *tuple);
  virtual void* Get(uint32_t *length);
  virtual bool Delete();
  virtual void SeekToFirst();
  virtual void SeekToLast();
  virtual void Next();
  virtual void Prev();
  virtual Status GetStatus();
private:
  RelationHandlerInterface *rel_handler_;
  Status status_;
  void* tuple_data_;
  uint32_t tuple_length_;
};


}  // namespace storage

#endif // ITERATOR_H_


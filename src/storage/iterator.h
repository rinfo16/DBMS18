#ifndef ITERATOR_H_
#define ITERATOR_H_

#include "storage_define.h"
#include "storage/iterator_interface.h"
#include "storage/buffer_manager.h"

namespace storage {

class Iterator : public IteratorInterface {
 public:
  Iterator(PageID rel_id, BufferManager *buffer_manager);
  ~Iterator();
  virtual bool Get(TupleWarpper *tuple);
  virtual const void* Get(uint32_t *length);
  virtual bool Delete();
  virtual void SeekToFirst();
  virtual void SeekToLast();
  virtual void Next();
  virtual void Prev();
  virtual Status GetStatus();
private:
  void SeekNext();
  PageID relation_id_;
  Status status_;
  const void* tuple_data_;
  uint32_t tuple_length_;
  Page *page_;
  uint32_t nth_slot_;
  BufferManager *buffer_manager_;
};


}  // namespace storage

#endif // ITERATOR_H_


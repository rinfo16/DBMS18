#ifndef ITERATOR_INTERFACE_H_
#define ITERATOR_INTERFACE_H_

#include "common/tuple.h"
#include "storage/io_object_interface.h"

namespace storage {

enum Status {
  kStatusInit,
  kStatusEOF,
  kStatusOK
};


class IteratorInterface : public IOObjectInterface{
 public:
  virtual ~IteratorInterface() {};
  virtual bool Get(TupleWarpper *tuple) = 0;
  virtual const void* Get(uint32_t *length) = 0;
  virtual bool Delete() = 0;
  virtual void SeekToFirst() = 0;
  virtual void SeekToLast() = 0;
  virtual void Next() = 0;
  virtual void Prev() = 0;
  virtual Status GetStatus() = 0;
};

}  // namespace storage

#endif // ITERATOR_INTERFACE_H_


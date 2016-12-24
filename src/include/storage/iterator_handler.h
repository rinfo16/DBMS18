#ifndef ITERATOR_INTERFACE_H_
#define ITERATOR_INTERFACE_H_

#include "common/tuple.h"
#include "leveldb/iterator.h"
#include "io_handler.h"

namespace storage {

enum Status {
  kStatusInit,
  kStatusEOF,
  kStatusOK
};


class IteratorHandler : public IOHandler{
 public:
  virtual ~IteratorHandler() {};

  virtual bool Valid() const = 0;

  virtual void SeekToFirst() = 0;

  virtual void SeekToLast() = 0;

  virtual void Seek(tupleid_t tupleid) = 0;

  virtual void Next() = 0;

  virtual void Prev() = 0;

  virtual tupleid_t TupleID() const = 0;

  virtual void GetTuple(TupleWarpper *tw) const = 0;
};

}  // namespace storage

#endif // ITERATOR_INTERFACE_H_


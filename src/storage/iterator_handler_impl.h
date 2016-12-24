#include "storage/iterator_handler.h"
#include "leveldb/db.h"

namespace storage {

class IteratorHandlerImpl : public IteratorHandler {
 public:
  IteratorHandlerImpl(const std::string & path);

  virtual ~IteratorHandlerImpl();

  virtual bool Valid() const;

  virtual void SeekToFirst();

  virtual void SeekToLast();

  virtual void Seek(tupleid_t tupleid);

  virtual void Next();

  virtual void Prev();

  virtual tupleid_t TupleID() const;

  virtual void GetTuple(TupleWarpper *tw) const;

private:
  leveldb::DB *db_;
  leveldb::Iterator *iter_;
};

}  // namespace storage



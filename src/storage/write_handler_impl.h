#ifndef WRITE_HANDLER_IMPL__
#define WRITE_HANDLER_IMPL__

#include "storage/write_handler.h"
#include "leveldb/db.h"
#include "leveldb/write_batch.h"

namespace storage {

class WriteHandlerImpl : public WriteHandler {
 public:
  WriteHandlerImpl(const std::string & path);
  void Put(const TupleWarpper *tuple);
  void Put(tupleid_t tuple_id, const TupleWarpper *tuple);
  void Delete(tupleid_t tuple_id);
  bool Commit();
 private:
  leveldb::DB *db_;
  leveldb::WriteBatch batch_;
};

}

#endif // WRITE_HANDLER_IMPL__

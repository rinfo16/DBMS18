#include "storage/write_handler_impl.h"
#include "uuid.h"

namespace storage {

WriteHandlerImpl::WriteHandlerImpl(const std::string & path)
    : db_(NULL) {
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, path, &db_);
  if (!status.ok()) {
    BOOST_LOG_TRIVIAL(error)<< "DB [" << path << " ] open error : " << status.ToString();
  }
  assert(status.ok());
  assert(db_);
}

WriteHandlerImpl::~WriteHandlerImpl()
{
  delete db_;
}
void WriteHandlerImpl::Put(const TupleWarpper *tuple) {
  Put(GenUUID(), tuple);
}

void WriteHandlerImpl::Put(tupleid_t tuple_id, const TupleWarpper *tuple) {
  leveldb::Slice key(tuple_id);
  leveldb::Slice value(tuple->Data(), tuple->Size());
  batch_.Put(key, value);
}

void WriteHandlerImpl::Delete(tupleid_t tuple_id) {
  leveldb::Slice key(tuple_id);
  batch_.Delete(key);
}

bool WriteHandlerImpl::Commit() {
  leveldb::Status status = db_->Write(leveldb::WriteOptions(), &batch_);
  if (!status.ok()) {
    BOOST_LOG_TRIVIAL(error)<< "DB write open error : " << status.ToString();
  }
  return status.ok();
}

}

#include "iterator_handler_impl.h"

namespace storage {

IteratorHandlerImpl::IteratorHandlerImpl(const std::string & path)
    : db_(NULL),
      iter_(NULL) {
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, path, &db_);
  if (!status.ok()) {
    BOOST_LOG_TRIVIAL(error)<< "DB [" << path << " ] open error : " << status.ToString();
  }
  assert(status.ok());
  assert(db_);
  iter_ = db_->NewIterator(leveldb::ReadOptions());
  assert(iter_);
}

IteratorHandlerImpl::~IteratorHandlerImpl() {
  // do not change the delete order ...
  delete iter_;
  delete db_;
}

bool IteratorHandlerImpl::Valid() const {
  return iter_->Valid();
}

void IteratorHandlerImpl::SeekToFirst() {
  iter_->SeekToFirst();
}

void IteratorHandlerImpl::SeekToLast() {
  iter_->SeekToLast();
}

void IteratorHandlerImpl::Seek(tupleid_t tupleid) {
  leveldb::Slice slice(tupleid);
  iter_->Seek(slice);
}

void IteratorHandlerImpl::Next() {
  iter_->Next();
}

void IteratorHandlerImpl::Prev() {
  iter_->Next();
}

tupleid_t IteratorHandlerImpl::TupleID() const {
  return std::string(iter_->key().data(), iter_->key().size());
}

void IteratorHandlerImpl::GetTuple(TupleWarpper *tw) const {
  assert(iter_->Valid());
  TupleWarpper tmp(iter_->value().data(), iter_->value().size());
  *tw = tmp;
}

}

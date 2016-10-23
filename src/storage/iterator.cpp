#include "iterator.h"
#include "stdlib.h"

namespace storage {

Iterator::Iterator(RelationHandlerInterface *rel_handler)
    : rel_handler_(rel_handler),
      tuple_data_(NULL),
      tuple_length_(0),
      status_(kStatusEOF) {
}

Iterator::~Iterator() {
  // TODO Auto-generated destructor stub
  delete rel_handler_;
}

bool Iterator::Get(TupleWarpper *tuple) {
  if (tuple_data_ == NULL)
    return false;

  // TODO  memory manager
  // the memory of Tuple::data_ will allocate from memory context
  void *ptr = malloc((size_t) tuple_length_);
  memcpy(ptr, tuple_data_, tuple_length_);
  TupleWarpper t((const uint8_t*) ptr, tuple_length_);
  *tuple = t;

  return true;
}

void* Iterator::Get(uint32_t *length) {
  *length = tuple_length_;
  return tuple_data_;
}

bool Iterator::Delete() {
  return rel_handler_->DeleteCurrent();
}
void Iterator::SeekToFirst() {
  tuple_data_ = (uint8_t*) rel_handler_->GetFirst(&tuple_length_);
  if (tuple_data_ == NULL) {
    status_ = kStatusEOF;
  } else {
    status_ = kStatusOK;
  }
}

void Iterator::SeekToLast() {
  // TODO
}

void Iterator::Next() {
  tuple_data_ = (uint8_t*) rel_handler_->GetNext(&tuple_length_);
  if (tuple_data_ == NULL) {
    status_ = kStatusEOF;
  } else {
    status_ = kStatusOK;
  }
}

void Iterator::Prev() {
  // TODO
}

Status Iterator::GetStatus() {
  return status_;
}

}  // namespace storage

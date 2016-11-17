#include "executor/seq_scan.h"
#include "storage/storage_service_interface.h"

namespace executor {

SeqScan::SeqScan(storage::IteratorInterface *iterator, int n)
    : iterator_(iterator),
      tuple_index_(n),
      rows_(0){
}

SeqScan::~SeqScan() {

}

bool SeqScan::Prepare() {
  return true;
}

bool SeqScan::Open() {
  iterator_->SeekToFirst();
  return true;
}

bool SeqScan::GetNext(TupleRow *row) {

  uint32_t length = 0;
  const void *p = iterator_->Get(&length);
  if (p) {
    rows_++;
    Tuple tuple = memory::CreateTuple(length);
    memcpy(tuple->Data(), p, length);
    row->SetTuple(tuple_index_, tuple);
    iterator_->Next();
  }
  return p != NULL;
}

void SeqScan::Close() {
}

}  // end namespace executor

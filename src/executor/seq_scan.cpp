#include "executor/seq_scan.h"
#include "storage/storage_service_interface.h"

namespace executor {

SeqScan::SeqScan(storage::IteratorHandler *iterator, int n)
    : iterator_(iterator),
      tuple_index_(n),
      rows_(0) {
}

SeqScan::~SeqScan() {

}

State SeqScan::Prepare() {
  return kStateOK;
}

State SeqScan::Open() {
  iterator_->SeekToFirst();
  return kStateOK;
}

State SeqScan::GetNext(TupleRow *row) {
  if (iterator_->Valid()) {
    rows_++;
    TupleWarpper t;
    iterator_->GetTuple(&t);
    Tuple tuple = memory::CreateTuple(t.Size());
    memcpy(tuple->Data(), t.Data(), t.Size());
    row->SetTuple(tuple_index_, tuple);
    iterator_->Next();
    return kStateOK;
  } else {
    return kStateEOF;
  }
}

void SeqScan::Close() {
}

}  // end namespace executor

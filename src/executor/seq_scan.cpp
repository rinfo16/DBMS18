#include "executor/seq_scan.h"
#include "storage/storage_service_interface.h"
#include <iostream>
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
  TupleWarpper tw;
  bool ok = iterator_->Get(&tw);
  if (ok) {
    rows_++;
    std::cout << rows_ << std::endl;
    row->SetTuple(tuple_index_, (Tuple*) tw.Data());
    iterator_->Next();
  }
  return ok;
}

void SeqScan::Close() {
}

}  // end namespace executor

#include "../storage/iterator_handler.h"
#include "executor/exec_interface.h"
namespace executor {

class SeqScan : public ExecInterface {
 public:
  SeqScan(storage::IteratorHandler *iterator, int n);
  ~SeqScan();
  // TODO .. filter
  // SeqScan(storage::Iterator *iterator, int n, Filter *filter);
  virtual State Prepare();
  virtual State Open();
  virtual State GetNext(TupleRow *row);
  virtual void Close();
 private:
  SeqScan();
  std::string table_name_;
  storage::IteratorHandler *iterator_;
  int tuple_index_;
  int rows_;
};
}

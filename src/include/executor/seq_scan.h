#include "executor/exec_interface.h"
#include "storage/iterator_interface.h"
namespace executor {

class SeqScan : public ExecInterface {
 public:
  SeqScan(storage::IteratorInterface *iterator, int n);
  ~SeqScan();
  // TODO .. filter
  // SeqScan(storage::Iterator *iterator, int n, Filter *filter);
  virtual bool Prepare();
  virtual bool Open();
  virtual bool GetNext(TupleRow *row);
  virtual void Close();
 private:
  SeqScan();
  std::string table_name_;
  storage::IteratorInterface *iterator_;
  int tuple_index_;
  int rows_;
};
}

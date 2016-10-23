
#ifndef TUPLE_H_
#define TUPLE_H_

#include "common/define.h"
#include <assert.h>

#include <string.h>
#include <string>

class Tuple {
 public:
  // Create an empty tuple.
  Tuple() : data_(NULL), size_(0) { }

  // Create a tuple that refers to d[0,n-1].
  Tuple(const uint8_t* d, size_t n) : data_(d), size_(n) { }

  // Return a pointer to the beginning of the referenced data
  const uint8_t* Data() const { return data_; }

  // Return the length (in bytes) of the referenced data
  size_t Size() const { return size_; }

  // Return true iff the length of the referenced data is zero
  bool Empty() const { return size_ == 0; }

private:
  const uint8_t *data_;
  size_t size_;
};

#endif  // TUPLE_H_

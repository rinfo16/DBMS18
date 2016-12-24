#include "common/tuple.h"

namespace memory {

Tuple CreateTuple(size_t size) {
  RawTuple *raw_tuple = reinterpret_cast<RawTuple*>(::malloc(size));
  return boost::shared_ptr<RawTuple>(raw_tuple, &free);
}

}

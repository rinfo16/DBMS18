#include "executor/hash.h"

namespace executor {

Hash::Hash(uint32_t bucket_count,
           ExecInterface *left,
           const std::vector<ValueExprInterface*> & hash_by_att) {

}

Hash::~Hash() {
}

}  // namespace executor

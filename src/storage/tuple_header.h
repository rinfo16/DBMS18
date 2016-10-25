#ifndef TUPLE_HEADER_H_
#define TUPLE_HEADER_H_

#include "common/define.h"

namespace storage {

typedef struct {
  uint16_t offset_;
  uint16_t length_;
  uint32_t create_trans_;
  uint32_t delete_trans_;
} TupleHeader;

}

#endif // TUPLE_HEADER_H_

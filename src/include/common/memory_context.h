#ifndef MEMORY_CONTEXT_H__
#define MEMORY_CONTEXT_H__

#include "postgres/palloc.h"

namespace memory {
  MemoryContext *SwitchToContext(MemoryContext *context);
  MemoryContext *CreateContext(MemoryContext *context, const char *name);
  void DestroyContext(MemoryContext *context);
  MemoryContext *RootContext();
  void *malloc(size_t t);
  void free(void *p);
};


#endif //MEMORY_CONTEXT_H__

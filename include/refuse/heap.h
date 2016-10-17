#ifndef REFUSE_HEAP_H
#define REFUSE_HEAP_H

#include <sys/types.h>
#include <stdint.h>

typedef struct _refuse_heap refuse_heap;

refuse_heap* refuse_heap_new();

void* refuse_heap_alloc(refuse_heap* heap, size_t bytes);

void refuse_heap_compact(refuse_heap* heap);
void refuse_heap_free(refuse_heap* heap, void* mem);
void refuse_heap_destroy(refuse_heap* heap);

size_t refuse_heap_allocated(refuse_heap* heap);

#endif // REFUSE_HEAP_H

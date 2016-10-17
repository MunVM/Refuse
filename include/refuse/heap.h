#ifndef REFUSE_HEAP_H
#define REFUSE_HEAP_H

#include <stddef.h>

typedef struct _refuse_heap refuse_heap;

refuse_heap* refuse_heap_new(size_t max_size);
void* refuse_heap_alloc(refuse_heap* heap, size_t size);
void refuse_heap_free(refuse_heap* heap, void* ptr);
void refuse_heap_destroy(refuse_heap* heap);
size_t refuse_heap_size(refuse_heap* heap);

#endif // REFUSE_HEAP_H

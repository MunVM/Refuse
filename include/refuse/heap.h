#ifndef REFUSE_HEAP_H
#define REFUSE_HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct _refuse_heap refuse_heap;

refuse_heap* refuse_heap_new(size_t size);

int refuse_heap_visit(refuse_heap* heap, void** ptr);
int refuse_heap_contains(refuse_heap* heap, void* ptr);

unsigned int refuse_heap_start(refuse_heap* heap);

void refuse_heap_destroy(refuse_heap* heap);
void refuse_heap_free(refuse_heap* heap, void* ptr);

void* refuse_heap_alloc(refuse_heap* heap, size_t size);

iendif // REFUSE_HEAP_H

#include <refuse/heap.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef unsigned char byte;

struct _refuse_heap{
  void* start;
  void* current;
  size_t size;
};

refuse_heap*
refuse_heap_new(size_t max_size){
  refuse_heap* heap = malloc(sizeof(refuse_heap));
  heap->start = heap->current = malloc(max_size);
  heap->size = max_size;
  return heap;
}

#define HEAP_SIZE(Heap) ((Heap)->current - (Heap)->start)
#define CHUNK_SIZE(Chunk) (*((size_t*)(Chunk)))

size_t
refuse_heap_size(refuse_heap* heap){
  return HEAP_SIZE(heap);
}

void*
refuse_heap_alloc(refuse_heap* heap, size_t size){
  size += sizeof(size_t);
  if((HEAP_SIZE(heap) + size) >= heap->size) return NULL;
  void* chunk = heap->current;
  heap->current += size;
  CHUNK_SIZE(chunk) = size;
  chunk += sizeof(size_t);
  memset(chunk, 0, size);
  return chunk;
}

void
refuse_heap_free(refuse_heap* heap, void* ptr){
  if(!((ptr >= heap->start) && (ptr <= (heap->start + heap->size)))) return;
  size_t size = CHUNK_SIZE(ptr - sizeof(size_t));
  printf("Freeing chunk of size: %li\n", size);
  memset(ptr, 0, size);
  CHUNK_SIZE(ptr) = 0x0;
}
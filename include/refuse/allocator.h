#ifndef REFUSE_ALLOCATOR_H
#define REFUSE_ALLOCATOR_H

#include "heap.h"

#ifndef GC_DEBUG
#define GC_DEBUG 0
#endif // GC_DEBUG

typedef enum{
  kEden,
  kSurvivor,
  kLargeObject
} generation_space;

typedef enum{
  kFree = 0,
  kWhite,
  kBlack,
  kGray
} chunk_color;

#define GC_MAX_REF 65536
#define GC_MIN_SIZE 256
#define SEMISPACE_SIZE ((1024 * 64) / 256)

typedef struct _root_chunk{
  struct _root_chunk* next;
  struct _root_chunk* prev;
  struct _alloc_chunk* chunk;
} root_chunk;

typedef struct{
  refuse_heap* eden;
  refuse_heap* survivor;
  refuse_heap* large;

  root_chunk* roots;
  struct _alloc_chunk* allocated;
  
  size_t num_semispace;
  void* semispace[SEMISPACE_SIZE];
} refuse_allocator;

refuse_allocator* refuse_allocator_new();

void refuse_allocator_gc_minor(refuse_allocator* alloc);
void refuse_allocator_gc_major(refuse_allocator* alloc);

void refuse_allocator_add_ref(refuse_allocator* alloc, void* start, void* end);
void refuse_allocator_del_ref(refuse_allocator* alloc, void* start, void* end);

static inline void
refuse_allocator_add_single_ref(refuse_allocator* alloc, void* ref){
  refuse_allocator_add_ref(alloc, ref, ref + sizeof(void*));
}

static inline void
refuse_allocator_del_single_ref(refuse_allocator* alloc, void* ref){
  refuse_allocator_del_ref(alloc, ref, ref + sizeof(void*));
}

void* refuse_allocator_alloc(refuse_allocator* alloc, size_t size);

#ifdef GC_DEBUG
void refuse_allocator_print_heap(refuse_allocator* alloc, generation_space space);
void refuse_allocator_print_refs(refuse_allocator* alloc);
#endif // GC_DEBUG

#endif // REFUSE_ALLOCATOR_H

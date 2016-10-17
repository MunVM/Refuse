#include <refuse/heap.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef unsigned char byte;

#define PAGE_SIZE (1024 * 4)

typedef struct _refuse_heap_node{
  struct _refuse_heap_node* next;
  size_t size;
} refuse_heap_node;

struct _refuse_heap{
  refuse_heap_node base;
  refuse_heap_node* free;
  refuse_heap_node* allocated;
  refuse_heap_node** bins;

  void* that;

  size_t parent_min_bytes;
  size_t recycle_bounds;
};

refuse_heap*
refuse_heap_new(){
  refuse_heap* heap = malloc(sizeof(refuse_heap));
  memset(heap, 0, sizeof(refuse_heap));
  if(heap != NULL){
    heap->free = &heap->base;
    heap->that = NULL;
    heap->parent_min_bytes = (1024 * 1024);
    heap->recycle_bounds = 1024;
    heap->bins = malloc(heap->recycle_bounds * sizeof(refuse_heap_node*));
    if(heap->bins != NULL){
      memset(heap->bins, 0, heap->recycle_bounds * sizeof(refuse_heap_node*));
    } else{
      refuse_heap_destroy(heap);
      heap = NULL;
    }
  }
  return heap;
}

void
refuse_heap_destroy(refuse_heap* heap){
  //TODO: Write destructor
}

#define ALIGN(size) ((size + sizeof(refuse_heap_node) - 1) / sizeof(refuse_heap_node))

static void
arena_free(refuse_heap* heap, void* mem){
  refuse_heap_node* a = ((refuse_heap_node*) (mem - 1));
  refuse_heap_node* f = heap->free;
  refuse_heap_node* n = f->next;

  for(;;){
    if((f < a && a < n) || (n <= f && (f < a || a < n))) break;
    f = n;
    n = f->next;
  }

  if(a + a->size == n){
    a->size += n->size;
    a->next = n->next;
  } else{
    a->next = n;
  }

  if(f + f->size == a){
    f->size += a->size;
    f->next = a->next;
  } else{
    f->next = a;
  }

  heap->free = f;
}

static refuse_heap_node*
alloc_pages(refuse_heap* heap, size_t size){
  if(size < (heap->parent_min_bytes / sizeof(refuse_heap_node))) size = (heap->parent_min_bytes / sizeof(refuse_heap_node));

  refuse_heap_node* page = malloc((size + 1) * sizeof(refuse_heap_node));
  if(page == NULL) return NULL;

  page->next = heap->allocated;
  heap->allocated = page;
  page->size = size + 1;

  refuse_heap_node* up = ((refuse_heap_node*) page + 1);
  up->size = size - 1;
  arena_free(heap, up + 1);
  return heap->free;
}

void*
refuse_heap_alloc(refuse_heap* heap, size_t size){
  size = ALIGN(size) + 1;
  refuse_heap_node* prev = heap->free;
  refuse_heap_node* next = prev->next;
  while(next->size < size){
    if(next == heap->free){
      next = alloc_pages(heap, size);
      if(next == NULL){
        return NULL;
      }
    }

    prev = next;
    next = next->next;
  }

  if(next->size == size){
    prev->next = next->next;
  } else{
    next->size -= size;
    next += next->size;
    next->size = size;
  }

  heap->free = prev;
  next->next = NULL;
  return (next + 1);
}

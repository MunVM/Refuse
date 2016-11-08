#include <refuse/heap.h>
<<<<<<< HEAD
#include <stdint.h>

typedef struct _refuse_heap_block{
  struct _refuse_heap_block* next;
  size_t size;
  void* ptr;
} refuse_heap_block;

struct _refuse_heap{
  void* start;
  void* ptr;
  size_t size;
  refuse_heap_block* unallocated;
  refuse_heap_block* allocated;
};

refuse_heap*
refuse_heap_new(size_t size){
  refuse_heap* heap = malloc(sizeof(refuse_heap));
  if(heap != NULL){
    heap->start = heap->ptr = malloc(size);
    heap->size = size;

    refuse_heap_block* filler = ((refuse_heap_block*) heap->ptr);
    heap->ptr += sizeof(refuse_heap_block);
    filler->size = size;
    filler->ptr = heap->ptr;
    filler->next = NULL;

    heap->allocated = NULL;
    heap->unallocated = filler;
  }

  return heap;
}

unsigned int
refuse_heap_start(refuse_heap* heap){
  return ((int) heap->start);
}

int
refuse_heap_visit(refuse_heap* heap, void** ptr){
  if((*ptr) == NULL){
    if(heap->allocated != NULL){
      *ptr = heap->allocated->ptr;
      return 1;
    } else{
      *ptr = NULL;
      return 0;
    }
  }

  refuse_heap_block* block = ((refuse_heap_block*) ((*ptr) - sizeof(refuse_heap_block)));
  if(block->next != NULL){
    block = block->next;
    *ptr = block->ptr;
    return 1;
  }  

  return 0;
}

int
refuse_heap_contains(refuse_heap* heap, void* ptr){
  return (ptr >= heap->start) &&
         (ptr <= (heap->start + heap->size));
}

void
refuse_heap_destroy(refuse_heap* heap){
  if(heap == NULL || heap->ptr == NULL) return;
  free(heap->ptr);
  free(heap);
}

static void
compact(refuse_heap* heap){
  refuse_heap_block* last = heap->unallocated;
  if(last->next != NULL){
    while(last->next != NULL) last = last->next;
    
    refuse_heap_block* this = heap->unallocated;
    refuse_heap_block* step = heap->allocated;

    while(this != last){
      while(step != NULL){
        if(step->ptr >= (this->ptr + this->size)) step->ptr -= this->size;
        step = step->next;
      }

      step = heap->allocated;

      last->ptr -= ((intptr_t) this->ptr);
      last->size += this->size;

      refuse_heap_block* free_block = this;
      this = this->next;
    }
  }
=======
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
>>>>>>> 5e7d205a1b0b6a9f2042f18b7d574dcef12d3db9
}

void*
refuse_heap_alloc(refuse_heap* heap, size_t size){
<<<<<<< HEAD
  size += sizeof(refuse_heap_block);
  if(heap != NULL && (size >= 0 && size <= heap->size)){
    size_t sorted = 0x0;
    refuse_heap_block* step = heap->unallocated;
    refuse_heap_block* chosen = NULL;

    while(sorted < 2){
      while(step != NULL){
        if(step->size >= size){
          if(chosen != NULL){
            if(step->size < chosen->size){
              chosen = step;
            }
          } else{
            chosen = step;
          }
        }

        step = step->next;
      }

      if(chosen != NULL){
        refuse_heap_block* new_block = ((refuse_heap_block*) chosen->ptr);
        chosen->ptr += sizeof(refuse_heap_block);
        new_block->size = size - sizeof(refuse_heap_block);
        new_block->ptr = chosen->ptr;    
        new_block->next = heap->allocated;
        heap->allocated = new_block;
        chosen->size = chosen->size - new_block->size;
        chosen->ptr += new_block->size;
        return new_block->ptr;
      } else{
        if(sorted < 1){
          compact(heap);
          step = heap->unallocated;
          sorted++;
        } else{
          sorted++;
        }
      }
    }

    return NULL;
  }
}

void
refuse_heap_free(refuse_heap* heap, void* ptr){
  refuse_heap_block* block = ((refuse_heap_block*) (ptr - sizeof(refuse_heap_block)));
  if(block != NULL){
    refuse_heap_block* alCheck = heap->unallocated;
    while(alCheck != NULL){
      if(block != alCheck){
        alCheck = alCheck->next;
      } else{
        return;
      }
    }

    refuse_heap_block* step = heap->allocated;
    if(step != block){
      while(step->next != NULL){
        if(step->next != block){
          step = step->next;
        } else{
          step->next = step->next->next;
        }
      }
    } else{
      heap->allocated = heap->allocated->next;
    }

    block->next = heap->unallocated;
    heap->unallocated = block;
    memset(block->ptr, 0, block->size);
  }
=======
  size += sizeof(size_t);
  if((HEAP_SIZE(heap) + size) >= heap->size) return NULL;
  void* chunk = heap->current;
  heap->current += size;
  CHUNK_SIZE(chunk) = size;
  chunk += sizeof(size_t);
  memset(chunk, 0, size);
  return chunk;
>>>>>>> 5e7d205a1b0b6a9f2042f18b7d574dcef12d3db9
}

void
refuse_heap_free(refuse_heap* heap, void* ptr){
  if(!((ptr >= heap->start) && (ptr <= (heap->start + heap->size)))) return;
  size_t size = CHUNK_SIZE(ptr - sizeof(size_t));
  printf("Freeing chunk of size: %li\n", size);
  memset(ptr, 0, size);
  CHUNK_SIZE(ptr) = 0x0;
}

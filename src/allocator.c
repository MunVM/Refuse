#include <refuse/allocator.h>
#include <stdio.h>

typedef unsigned char byte;

typedef struct _alloc_chunk alloc_chunk;

typedef struct{
  alloc_chunk** table;
  size_t size;
  size_t asize;
} reference_table;

reference_table*
reference_table_new(){
  reference_table* table = malloc(sizeof(reference_table));
  table->size = 0;
  table->asize = 0xA;
  table->table = malloc(sizeof(alloc_chunk*) * 0xA);
  return table;
}

struct _alloc_chunk{
  alloc_chunk* next;
  alloc_chunk* prev;
  reference_table* references;
  size_t size;
  chunk_color color;
  byte marked : 1;
};

static inline void*
alloc_chunk_new(void* memory, size_t size){
  alloc_chunk* chunk = ((alloc_chunk*) memory);
  chunk->marked = 0;
  chunk->size = size;
  chunk->color = kFree;
  chunk->references = reference_table_new();
  return ((void*) (memory + sizeof(alloc_chunk)));
}

static void
alloc_chunk_add_reference(alloc_chunk* owner, alloc_chunk* reference){
  reference_table* owner_table = owner->references;
  if(owner_table->size + 1 >= owner_table->asize){
    alloc_chunk** new_table = realloc(owner_table->table, owner_table->asize * 2);
    owner_table->table = new_table;
    owner_table->asize *= 2;
  }
  owner_table->table[owner_table->size++] = reference;
}

refuse_allocator*
refuse_allocator_new(){
  refuse_allocator* alloc = malloc(sizeof(refuse_allocator));
  alloc->eden = refuse_heap_new(1024 * 64);
  alloc->survivor = refuse_heap_new(1024 * 64);
  alloc->large = refuse_heap_new(1024 * 1024 * 32);
  alloc->roots = NULL;
  alloc->allocated = NULL;
  memset(alloc->semispace, 0, sizeof(alloc->semispace));
  return alloc;
}

static void* major_alloc(refuse_allocator* alloc, size_t size);
static void* minor_alloc(refuse_allocator* alloc, size_t size);

void*
refuse_allocator_alloc(refuse_allocator* alloc, size_t size){
  void* ptr = minor_alloc(alloc, size);
  if(ptr == NULL) return major_alloc(alloc, size);
  return ptr;
}

#define WITH_HEADER(size) ((size) + sizeof(int))
#define WITHOUT_HEADER(size) ((size) - sizeof(int))
#define ALIGN(ptr) (((ptr) + 3) & ~3)
#define FLAGS(chunk) (*((unsigned int*)(chunk)))
#define CHUNK_FLAGS(chunk) (FLAGS((chunk)) & (3))
#define CHUNK_SIZE(chunk) (FLAGS((chunk)) & (~3))
#define PTR_INDEX(ofs) ((ofs) /  sizeof(void*))
#define BITS(chunk) (WITH_HEADER(chunk))
#define MEM_TAG(ptr) (!(((unsigned int)(ptr)) & 1))
#define BITS_AT(chunk, idx) ((((void**)(BITS(chunk)) + (idx) * sizeof(void*))))
#define REF_PTR_MIN(ptr) (MEM_TAG((ptr)) && refuse_heap_contains(alloc->eden, (ptr)))
#define MARKED(chunk) (FLAGS(chunk) & 1)
#define MARK_CHUNK(chunk, c) ((FLAGS(chunk)) = CHUNK_SIZE(chunk)|c)
#define CHUNK_OFFSET(chunk) ((chunk) - refuse_heap_start(alloc->eden))

static void*
minor_alloc(refuse_allocator* alloc, size_t size){
  if(size == 0) return NULL;
  size += sizeof(alloc_chunk);
  if(size > GC_MIN_SIZE){
    return major_alloc(alloc, size - sizeof(alloc_chunk));
  }

  void* raw = refuse_heap_alloc(alloc->eden, size);
  if(raw == NULL){
    //TODO: Scavenge Heap
    return minor_alloc(alloc, size - sizeof(alloc_chunk));
  }
  void* bits = alloc_chunk_new(raw, size - sizeof(alloc_chunk));

  alloc_chunk* c = ((alloc_chunk*) raw);
  c->next = alloc->allocated;
  if(alloc->allocated != NULL) alloc->allocated->prev = c;
  alloc->allocated = c;

  return bits;
}

static void*
major_alloc(refuse_allocator* alloc, size_t size){
  return NULL;
}

void
refuse_allocator_add_ref(refuse_allocator* alloc, void* owner, void* ref){
  alloc_chunk* owner_chunk = ((alloc_chunk*) (owner - sizeof(alloc_chunk)));
  alloc_chunk* ref_chunk = ((alloc_chunk*) (ref - sizeof(alloc_chunk)));
  alloc_chunk_add_reference(owner_chunk, ref_chunk);

  root_chunk* root = alloc->roots;
  while(root != NULL && root->chunk != owner_chunk) root = root->next;

  if(root == NULL){
    root = malloc(sizeof(root_chunk));
    root->chunk = owner_chunk;
    root->next = alloc->roots;
    if(alloc->roots != NULL) alloc->roots->prev = root;
    alloc->roots = root;
  }
}

void
refuse_allocator_del_ref(refuse_allocator* alloc, void* begin, void* end){
  //TODO: Implement
}

static inline void
mark_chunk(refuse_allocator* alloc, alloc_chunk* c){
  c->color = kWhite;
  c->marked = 1;

  reference_table* refs = c->references;
  if(refs != NULL){
    for(int i = 0; i < refs->size; i++){
      mark_chunk(alloc, refs->table[i]);
    }
  }
}

static inline alloc_chunk*
promote(refuse_allocator* alloc, alloc_chunk* chunk){
  if(chunk == NULL) return NULL;
  alloc_chunk* new_chunk = refuse_heap_alloc(alloc->survivor, chunk->size + sizeof(alloc_chunk));
  void* new_raw = alloc_chunk_new(new_chunk, chunk->size);
  memset(new_raw, 0, chunk->size);
  return new_chunk;
}

static void
update_refs(refuse_allocator* alloc){
  //TODO: Update References?
}

void
refuse_allocator_gc_minor(refuse_allocator* alloc){
  alloc->num_semispace = 0x0;
  memset(alloc->semispace, 0, sizeof(alloc->semispace));

  root_chunk* root = alloc->roots;
  while(root != NULL){
    if(refuse_heap_contains(alloc->eden, root->chunk)){
      mark_chunk(alloc, root->chunk);
    }

    root = root->next;
  }

  void* ptr = NULL;
  while(refuse_heap_visit(alloc->eden, &ptr)){
    alloc_chunk* chunk = ((alloc_chunk*) ptr);
    if(chunk->marked){
      alloc_chunk* new_chunk = promote(alloc, chunk);
      alloc->semispace[alloc->num_semispace++] = new_chunk;
    }
  }

  ptr = NULL;
  int offset = 0x1;
  while(refuse_heap_visit(alloc->eden, &ptr)){
    alloc_chunk* chunk = ((alloc_chunk*) ptr);
    if(chunk->marked){
      alloc_chunk* new_chunk = alloc->semispace[alloc->num_semispace - (offset++)];
      void* new_bits = ((void*) (new_chunk + sizeof(alloc_chunk)));
      void* old_bits = ((void*) (chunk + sizeof(alloc_chunk)));
      memcpy(new_bits, old_bits, chunk->size);

      if(chunk->prev) chunk->prev->next = chunk->next;
      if(chunk->next) chunk->next->prev = chunk->prev;

      refuse_heap_free(alloc->eden, chunk);
      ptr = NULL;
    }
  }
}

#ifdef GC_DEBUG

void
refuse_allocator_print_heap(refuse_allocator* alloc, generation_space space){
  refuse_heap* heap = NULL;
  switch(space){
    case kEden: {
      printf("*** List of heap allocated chunks\n");
      printf("*** Generation: Eden\n");
      heap = alloc->eden; break;
    }
    case kSurvivor: {
      printf("*** List of heap allocated chunks\n");
      printf("*** Generation: Survivor\n");
      heap = alloc->survivor; break;
    }
    case kLargeObject: {
      printf("*** List of heap allocated chunks\n");
      printf("*** Generation: Large\n");
      heap = alloc->large; break;
    }
    default: {
      fprintf(stderr, "Unreachable\n");
      abort();
    }
  }

  int count = 0x0;
  void* ptr = NULL;
  while(refuse_heap_visit(heap, &ptr)){
    alloc_chunk* chunk = ((alloc_chunk*) ptr);
    printf("  Chunk: %p; Size: %.3d; Marked: %c\n",
          ((void*) (chunk - sizeof(alloc_chunk))),
          chunk->size,
          (chunk->marked ? 'y' : 'n')
    );

    count += 1;
  }

  printf("*** %d Chunks\n", count);
  printf("*** End of heap chunk list\n");
}

void
refuse_allocator_print_refs(refuse_allocator* alloc){
  printf("*** List of stored references\n");

  printf("*** End of stored references\n");
}

#endif // GC_DEBUG

#include <refuse/heap.h>
#include <stdio.h>

#define GC_DEBUG 1

#include <refuse/allocator.h>

typedef struct{
  int x;
  int y;
} point;

static inline void
print_point(point* p){
  printf("%p(%d, %d)\n", p, p->x, p->y);
}

int
main(int argc, char** argv){
  refuse_allocator* alloc = refuse_allocator_new();
  
  point* p1 = refuse_allocator_alloc(alloc, sizeof(point));
  p1->x = 10;
  p1->y = 10;

  point* p2 = refuse_allocator_alloc(alloc, sizeof(point));
  p2->x = 100;
  p2->y = 100;

  refuse_allocator_add_ref(alloc, p1, p2);

  refuse_allocator_gc_minor(alloc);
  
  refuse_allocator_print_heap(alloc, kEden);
  refuse_allocator_print_heap(alloc, kSurvivor);

  p1->x = 10000;
  p1->y = 10000;

  p2->x = 100000;
  p2->y = 100000;

  print_point(p1);
  print_point(p2);

  p1->x = 1000;
  p1->y = 1000;
  p2->x = 10000;
  p2->y = 10000;

  refuse_allocator_gc_minor(alloc);

  refuse_allocator_print_heap(alloc, kEden);
  printf("---------------------------------------------------------\n");
  refuse_allocator_print_heap(alloc, kSurvivor);

  print_point(p1);
  print_point(p2);

  return 0x0;
}

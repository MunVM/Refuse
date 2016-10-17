#include <stdio.h>
#include <refuse/heap.h>

typedef struct{
  int x;
  int y;
} point;

static inline void
print_point(point* p){
  printf("(%d, %d)\n", p->x, p->y);
}

int
main(int argc, char** argv){
  refuse_heap* heap = refuse_heap_new();

  point* p1 = refuse_heap_alloc(heap, sizeof(point));
  p1->x = 10;
  p1->y = 10;

  point* p2 = refuse_heap_alloc(heap, sizeof(point));
  p2->x = 100;
  p2->y = 100;

  print_point(p1);
  print_point(p2);  
  
  return 0;
}

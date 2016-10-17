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
  refuse_heap* heap = refuse_heap_new(1024 * 4);

  point* p1 = refuse_heap_alloc(heap, sizeof(point));
  p1->x = 10;
  p1->y = 10;

  point* p2 = refuse_heap_alloc(heap, sizeof(point));
  p2->x = 100;
  p2->y = 100;

  print_point(p1);
  print_point(p2);

  refuse_heap_free(heap, p1);
  refuse_heap_free(heap, p2);

  print_point(p2);
  print_point(p1);

  point* p3 = refuse_heap_alloc(heap, sizeof(point));
  p3->x = 100;
  p3->y = 100;
  print_point(p3);

  refuse_heap_free(heap, p3);

  print_point(p3);
  return 0;
}

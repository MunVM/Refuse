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
  return 0;
}

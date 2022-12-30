// This is code for a simple and performant ring buffer queue 
// that uses an array as it's underlying data structure.
//
// The 'count' variable is mostly used to avoid having special 
// states for 'front' and 'rear' variable.
//

#include "momo.h"
#include <stdio.h>


struct gist_rb {
  int values[4];

  u32_t count;
  u32_t front;
  u32_t rear;
};

static int* 
gist_rb_enqueue(gist_rb* rb) 
{
  if (rb->count >= array_count(rb->values)) return nullptr;

  int* ret = rb->values + rb->rear++;
  rb->rear %= array_count(rb->values);
  ++rb->count;
  return ret;

}

static int* 
gist_rb_dequeue(gist_rb* rb) 
{
  if (rb->count == 0) return nullptr;

  int* ret = rb->values + rb->front++;
  rb->front %= array_count(rb->values);
  --rb->count;
  return ret;
}

int main() 
{
  // can just init with {0}
  gist_rb ring = {0};

  int* i = 0;

  *gist_rb_enqueue(&ring) = 1 ;
  *gist_rb_enqueue(&ring) = 2;
  
  printf("%d ", *gist_rb_dequeue(&ring));
  printf("%d ", *gist_rb_dequeue(&ring));

  *gist_rb_enqueue(&ring) = 3;
  *gist_rb_enqueue(&ring) = 4;
   
  printf("%d ", *gist_rb_dequeue(&ring));
  printf("%d ", *gist_rb_dequeue(&ring));

  // after this, it should loop around the queue
  *gist_rb_enqueue(&ring) = 5;
  *gist_rb_enqueue(&ring) = 6;
  *gist_rb_enqueue(&ring) = 7;
  *gist_rb_enqueue(&ring) = 8;
   
  printf("%d ", *gist_rb_dequeue(&ring));
  printf("%d ", *gist_rb_dequeue(&ring));
  printf("%d ", *gist_rb_dequeue(&ring));
  printf("%d ", *gist_rb_dequeue(&ring));




}



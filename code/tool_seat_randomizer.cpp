#include "momo.h"

#include <stdio.h>


struct tsr_seat_t
{
  u32_t student_id;
};

struct tsr_room_t
{
};


int main () 
{
  arena_t arena = {};
  rng_t rng = {};
  rng_init(0); // todo: test
  arena_alloc(&arena, gigabytes(1));
  defer { arena_free(&arena); };

  const u32_t rows = 6;
  const u32_t cols = 20;
  const u32_t student_count = 50;
  const u32_t seat_count = rows * cols;

  if (seat_count < student_count) printf("seats cannot be less than students!");
      
  u32_t* seats = arena_push_arr(u32_t, &arena, seat_count);
  if (!seats) printf("cannot allocate seats!");
  u32_t* students = arena_push_arr(u32_t, &arena, student_count);
  if (!students) printf("cannot allocate students!");

  // Initialize the students
  for_cnt(student_index, student_count)
    students[student_index] = student_index;

  // Randomize the students
  for(s32_t i = student_count - 1; i > 0; --i)
  {
    u32_t j = rng_range_u32(&rng, 0, i);
    swap(&students[i], &students[j]);

  }
  
}

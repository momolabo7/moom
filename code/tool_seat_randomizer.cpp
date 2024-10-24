#include "momo.h"

#include <stdio.h>


struct tsr_seat_t
{
  u32_t student_id;
};


int main () 
{
  arena_t arena = {};
  rng_t rng = {};
  rng_init(&rng, 1); // todo: test
  arena_alloc(&arena, gigabytes(1));
  defer { arena_free(&arena); };

  u32_t rows = 6;
  u32_t cols = 20;
  u32_t student_count = 50;
  u32_t seat_count = rows * cols;

  if (seat_count < student_count) printf("seats cannot be less than students!");
      
  u32_t* seats = arena_push_arr(u32_t, &arena, seat_count);
  if (!seats) printf("cannot allocate seats!");
  u32_t* students = arena_push_arr(u32_t, &arena, student_count);
  if (!students) printf("cannot allocate students!");

  // Initialize the students
  for_cnt(student_index, student_count)
  {
    students[student_index] = student_index;
  }

  // Randomize the students
  for(s32_t i = student_count - 1; i > 0; --i)
  {
    u32_t j = rng_range_u32(&rng, 0, i);
    swap(students[i], students[j]);
  }

#if 0
  for_cnt(student_index, student_count)
  {
    printf("%d ", students[student_index]);
  }
#endif

  // algorithm to seat students
  // students must be one table apart.

  u32_t student_index = 0;
  u32_t seat_index = 0;
  for(;;)
  {
    if (seat_index >= seat_count)
    {
      break;
    }
    // check neighbours of current seat we are slotting.
    u32_t seat_row = seat_index / rows;
    u32_t seat_col = seat_index % rows;
    if (seat_row > 0) {
    }

    printf("%d %d\n", seat_row, seat_col);
    ++seat_index;

  }
  
  // print seats
  for_cnt(r, rows)
  {
    for_cnt(c, cols)
    {
      printf("%2d ", seats[c + r * cols]);
    }
    printf("\n");
  }
  printf("\n");
}

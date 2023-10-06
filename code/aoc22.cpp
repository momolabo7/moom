#include "momo.h"
#include <stdio.h>
#include <stdlib.h>


static void 
aoc22_d1a(const char* filename) 
{
  u32_t max = 0;
  u32_t sum = 0;
  FILE* fp = fopen(filename, "r");
  char buffer[64];
  if (fp) {
    while(fgets(buffer, array_count(buffer), fp)) {
      if (buffer[0] == '\n' || buffer[0] == '\r') {
        //printf("sum of elf: %d\n", sum); 
        if (sum > max) {
          //printf("MAX DETECTED!\n");
          max = sum;
        }
        sum = 0;
      }
      else {
        sum += cstr_to_u32(buffer);
        //printf("sum is now: %d\n", sum); 
      }
    }

    //printf("fattest elf: %d\n", max);
    printf("%d", max);
    fclose(fp);
  }
}

// We should really really do a nested switch case
// for this question but using numbers is fun.
static u32_t 
aoc22_d2_calc_score(u32_t lhs, int rhs) {
  if (lhs == 0) {
    if (rhs == 0) return 3;
    else if (rhs == 1) return 0;
    else if (rhs == 2) return 6;
  }
  else if (lhs == 1) {
    if (rhs == 0) return 6;
    else if (rhs == 1) return 3;
    else if (rhs == 2) return 0;
  }
  else // (lhs == 2)
  {
    if (rhs == 0) return 0;
    else if (rhs == 1) return 6;
    else if (rhs == 2) return 3;
  }
  
  return 0;
}

static void
aoc22_d2a(const char* filename) 
{
  u32_t score = 0;
  FILE* fp = fopen(filename, "r");
  const u32_t buffer_size = 64;
  char buffer[buffer_size];
  if (fp) {
    while(fgets(buffer, buffer_size, fp)) {
      u32_t them = buffer[0] - 'A';

      // we reduce us to match them
      // i.e. X Y Z becomes A B C

      u32_t us = buffer[2] - 'X'; 

      score += us + 1;
      
      // Draw condition is when we match
      //   e.g. A vs A, B vs B, C vs C
      // Win/Lose condition
      //   Since:
      //     A = Rock
      //     B = Paper
      //     C = Scissors
      //   Then:
      //     A lose to B lose to C lose to A
      //   
      //   
      u32_t win_score = aoc22_d2_calc_score(us, them);
      //printf("%c vs %c: %d + %d\n", them+'A', us+'A', us+1, win_score);
      score += win_score; 
      //printf("Current score: %d\n", score);
    }

    //printf("Total score: %d\n", score);

    printf("%d", score);

    fclose(fp);
  }


}


static void 
aoc22_d2b(const char* filename) 
{
  u32_t score = 0;
  FILE* fp = fopen(filename, "r");
  const u32_t buffer_size = 64;
  char buffer[buffer_size];
  if (fp) {
    while(fgets(buffer, buffer_size, fp)) {
      u32_t them = buffer[0] - 'A';

      // we reduce us to match them
      // i.e. X Y Z becomes A B C
      u32_t condition = buffer[2];

      u32_t us = 0;
      switch(condition) {
        case 'X': {
          // Need to lose
          us = them - 1;
          if (us == -1) us = 2;
        } break;
        case 'Y': {
          // Need to draw
          us = them;
        } break;
        case 'Z': {
          // Need to win
          us = them + 1;
          if (us == 3) us = 0;
        } break;


      }

      score += us + 1;
      
      // Draw condition is when we match
      //   e.g. A vs A, B vs B, C vs C
      // Win/Lose condition
      //   Since:
      //     A = Rock
      //     B = Paper
      //     C = Scissors
      //   Then:
      //     A lose to B lose to C lose to A
      //   
      //   
      u32_t win_score = aoc22_d2_calc_score(us, them);
      //printf("%c vs %c: %d + %d\n", them+'A', us+'A', us+1, win_score);
      score += win_score; 
      

      //printf("Current score: %d\n", score);

    }

    //printf("Total score: %d\n", score);
    printf("%d", score);

    fclose(fp);
  }



}

static u32_t 
aoc22_d3_get_priority(char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 'a' + 1;
  }
  if (c >= 'A' && c <= 'Z') {
    return c - 'A' + 27;
  }
  return 0;
}

static void 
aoc22_d3a(const char* filename) {
  u32_t score = 0;
  FILE* fp = fopen(filename, "r");
  char buffer[128];
  if (fp) {
    u32_t priority = 0;
    while(fgets(buffer, sizeof(buffer), fp)) {
      // NOTE(momo): There are no odd cases!
      u32_t len = cstr_len_if(buffer, is_alpha);      
      u32_t split = len/2;

      // Find first common item 
      char common = 0;
      for (u32_t i = 0; i < split; ++i) 
      {
        for(u32_t j = split; j < len; ++j)
        {
          if (buffer[i] == buffer[j]){
            common = buffer[i];
            goto found;
          }

        }
      }

found:
      priority = aoc22_d3_get_priority(common); 
      //printf("#%d: common is %c: %d! (%d, %d)\n", i, common, priority, split, len);
      score += priority; 
    }

    //printf("sum: %d\n", sum);
    printf("%d", score);
    fclose(fp);
  }
}

static void 
aoc22_d3b(const char* filename) 
{
  u32_t score = 0;
  FILE* fp = fopen(filename, "r");
  char buffer[3][128];

  if (fp) {
    u32_t rucksack_index = 0;
    u32_t priority = 0;
    // NOTE(momo): There are exactly 300 lines
    while(1) {
      char common = 0;
      // Fill buffers
      for (u32_t i = 0; i < 3; ++i)
      {
        if(!fgets(buffer[i], sizeof(buffer[i]), fp)) {
          goto finished;
        }
      }

      // search for common shit between all 3 bags
      for (char* it0 = buffer[0]; is_alpha(*it0); ++it0)
      {
        for(char* it1 = buffer[1]; is_alpha(*it1); ++it1)
        {
          for(char* it2 = buffer[2]; is_alpha(*it2); ++it2)
          {
            if (*it0 == *it1 && *it1 == *it2) {
              common = *it0;
              goto found;
            }

          }

        }
      }

found:
      score += aoc22_d3_get_priority(common); 

    }

finished:

    printf("%d", score);
    fclose(fp);
  }


}

static void
aoc22_d4a(const char* filename) {

  u32_t overlapping_pairs =  0;; 
  FILE* fp = fopen(filename, "r");
  char buffer[16];
  if (fp) {

    while(fgets(buffer, sizeof(buffer), fp)) {
      tokenizer_t tokenizer = { buffer, 0 };
      r2u_t first_elf;
      r2u_t second_elf;

      first_elf.min = tokenizer_get_next_u32(&tokenizer);
      first_elf.max = tokenizer_get_next_u32(&tokenizer);

      second_elf.min = tokenizer_get_next_u32(&tokenizer);
      second_elf.max = tokenizer_get_next_u32(&tokenizer);

      u32_t overlap = r2u_is_fully_overlapping(first_elf, second_elf);
#if 0
      printf("%d-%d,%d-%d: %d\n", 
             first_elf.min, first_elf.max, 
             second_elf.min, second_elf.max, 
             overlap);
#endif

      overlapping_pairs += overlap;


    }
    printf("%d", overlapping_pairs);
    fclose(fp);
  }
}

static void
aoc22_d4b(const char* filename) {

  u32_t overlapping_pairs =  0;; 
  FILE* fp = fopen(filename, "r");
  char buffer[16];
  if (fp) {
    while(fgets(buffer, sizeof(buffer), fp)) {
      tokenizer_t tokenizer = { buffer, 0 };
      r2u_t first_elf;
      r2u_t second_elf;

      first_elf.min = tokenizer_get_next_u32(&tokenizer);
      first_elf.max = tokenizer_get_next_u32(&tokenizer);

      second_elf.min = tokenizer_get_next_u32(&tokenizer);
      second_elf.max = tokenizer_get_next_u32(&tokenizer);

      u32_t overlap = r2u_is_overlapping(first_elf, second_elf); 
#if 0
      printf("%d-%d,%d-%d: %d\n", 
             first_elf.min, first_elf.max, 
             second_elf.min, second_elf.max, 
             overlap);
#endif

      overlapping_pairs += overlap;


    }

    //printf("Overlapping pairs: %d\n", overlapping_pairs);
    printf("%d", overlapping_pairs);
    fclose(fp);
  }



}

int main() {
}

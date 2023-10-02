// 
// TODO: parse negative numbers correctly
//

#include <stdio.h>
#include "momo.h"

#if 1 // FOOLISH

#include <stdlib.h>

//
// MARK:(Foolish)
//
// These are foolish methods that should ideally not make 
// it into any SERIOUS projects. They are meant to be dumb
// but convienient for trying stuff out.
//
static void* 
foolishly_allocate_size(usz_t size) {
  return malloc(size); 
}

static void 
foolishly_free_memory(void* mem) {
  free(mem);
}
#define foolishly_allocate(t)  (t*)foolishly_allocate_size(sizeof(t))
#define foolishly_allocate_array(t,n) (t*)foolishly_allocate_size(sizeof(t) * (n))

static buffer_t
foolishly_read_file_into_buffer(const char* filename, b32_t null_terminate = false) {
  FILE *file = fopen(filename, "rb");
  if (!file) return buffer_set(0,0);
  defer { fclose(file); };

  fseek(file, 0, SEEK_END);
  usz_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer_t ret;
  ret.size = file_size + null_terminate; // lol
  ret.data = (u8_t*)foolishly_allocate_size(ret.size);

  usz_t read_amount = fread(ret.data, 1, file_size, file);
  if(read_amount != file_size) return buffer_set(0,0);

  if (null_terminate) ret.data[file_size] = 0;  

  return ret;
}

static void
foolishly_free_buffer(buffer_t buffer) {
  free(buffer.data);
}

#endif // FOOLISH







static void 
meta_print_if_function_is_next(clex_tokenizer_t t) {
  clex_token_t token_a =  clex_next_token(&t);
  clex_token_t token_b =  clex_next_token(&t);
  clex_token_t token_c =  clex_next_token(&t);
  if (token_a.type == CLEX_TOKEN_TYPE_IDENTIFIER &&
      token_b.type == CLEX_TOKEN_TYPE_IDENTIFIER &&
      token_c.type == CLEX_TOKEN_TYPE_OPEN_PAREN)
  {
    // search for close brace.
    clex_token_t end_token;
    do {
      end_token = clex_next_token(&t);
    } while(end_token.type != CLEX_TOKEN_TYPE_CLOSE_PAREN);

    // TODO: Check for EOF

    clex_token_t final_token = clex_next_token(&t);
    
    clex_token_t function_as_token;
    function_as_token.begin = token_a.begin;
    function_as_token.ope = end_token.ope;

    if (final_token.type == CLEX_TOKEN_TYPE_SEMICOLON) {
      printf("Declaration: ");
    }
    else if(final_token.type == CLEX_TOKEN_TYPE_OPEN_BRACE) {
      printf("Definition: ");
    }
    else return;
    meta_print_token(&t, function_as_token);

    printf("\n");
  }
}

// In this program
int main() {
  make(clex_tokenizer_t, t);
  buffer_t buffer = foolishly_read_file_into_buffer("../code/momo.h", true);
  defer { foolishly_free_buffer(buffer); };

  clex_tokenizer_init(t, buffer);

  for(;;) {
    clex_token_t token = clex_next_token(t);
    if (token.type == CLEX_TOKEN_TYPE_EOF) 
      break;
    meta_print_if_function_is_next(*t);
  } 
}

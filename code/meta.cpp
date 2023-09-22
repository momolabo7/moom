// 
// TODO: parse negative numbers correctly
//

#include <stdio.h>
#include "momo.h"

enum meta_token_type_t {
  META_TOKEN_TYPE_UNKNOWN,
  META_TOKEN_TYPE_OPEN_PAREN,          // (
  META_TOKEN_TYPE_CLOSE_PAREN,         // )
  META_TOKEN_TYPE_SEMICOLON,           // ;
  META_TOKEN_TYPE_COMMA,               // ,
  META_TOKEN_TYPE_DOT,                 // ,
  META_TOKEN_TYPE_COLON,               // : 
  META_TOKEN_TYPE_SCOPE,               // :: 
  META_TOKEN_TYPE_OPEN_BRACKET,        // [
  META_TOKEN_TYPE_CLOSE_BRACKET,       // ]
  META_TOKEN_TYPE_OPEN_BRACE,          // {
  META_TOKEN_TYPE_CLOSE_BRACE,         // } 
  META_TOKEN_TYPE_PLUS,                // +
  META_TOKEN_TYPE_PLUS_PLUS,           // ++ 
  META_TOKEN_TYPE_PLUS_EQUAL,          // += 
  META_TOKEN_TYPE_MINUS,               // -
  META_TOKEN_TYPE_MINUS_MINUS,         // -- 
  META_TOKEN_TYPE_MINUS_EQUAL,         // -= 
  META_TOKEN_TYPE_ARROW,               // ->
  META_TOKEN_TYPE_EQUAL,               // =
  META_TOKEN_TYPE_EQUAL_EQUAL,         // ==
  META_TOKEN_TYPE_GREATER,             // > 
  META_TOKEN_TYPE_GREATER_EQUAL,       // >= 
  META_TOKEN_TYPE_GREATER_GREATER,     // >>
  META_TOKEN_TYPE_LESSER,              // < 
  META_TOKEN_TYPE_LESSER_EQUAL,        // <= 
  META_TOKEN_TYPE_LESSER_LESSER,       // <<
  META_TOKEN_TYPE_OR,                  // |
  META_TOKEN_TYPE_OR_EQUAL,            // |=
  META_TOKEN_TYPE_OR_OR,               // ||
  META_TOKEN_TYPE_AND,                 // & 
  META_TOKEN_TYPE_AND_AND,             // &&
  META_TOKEN_TYPE_AND_EQUAL,           // &=
  META_TOKEN_TYPE_LOGICAL_NOT,         // !
  META_TOKEN_TYPE_BITWISE_NOT,         // ~
  META_TOKEN_TYPE_XOR,                 // ^
  META_TOKEN_TYPE_XOR_EQUAL,           // ^=
  META_TOKEN_TYPE_QUESTION,            // ?
  META_TOKEN_TYPE_STAR,                // *
  META_TOKEN_TYPE_STAR_EQUAL,          // *=
  META_TOKEN_TYPE_SLASH,               // /
  META_TOKEN_TYPE_SLASH_EQUAL,         // /=
  META_TOKEN_TYPE_PERCENT,             // %
  META_TOKEN_TYPE_PERCENT_EQUAL,       // %=
  META_TOKEN_TYPE_IDENTIFIER,
  META_TOKEN_TYPE_KEYWORD,
  META_TOKEN_TYPE_STRING,
  META_TOKEN_TYPE_NUMBER,
  META_TOKEN_TYPE_CHAR,
  META_TOKEN_TYPE_MACRO,
  META_TOKEN_TYPE_EOF
};

struct meta_token_t {
  meta_token_type_t type;

  usz_t begin;
  usz_t ope;
};

struct meta_tokenizer_t {
  buffer_t text;
  usz_t at;
#if 0
  c8_t* text;
  u32_t at;
  u32_t text_length;
#endif
};


static void
meta_eat_ignorables(meta_tokenizer_t* t) {
  for (;;) {
    if(is_whitespace(t->text.data[t->at])) {
      ++t->at;
    }
    else if(t->text.data[t->at] == '/' && t->text.data[t->at+1] == '/')  // line comments
    {
      while(t->text.data[t->at] != '\n') {
        ++t->at;
      }
    }
    else if(t->text.data[t->at] == '/' && t->text.data[t->at+1] == '*')  // block comments
    {
      t->at += 3;
      while(t->text.data[t->at] != '*' && t->text.data[t->at+1] != '/') {
        ++t->at;
      }

    }
    else {
      break;
    }
  }

}

static b32_t 
meta_is_accepted_character_for_number(char c) {
  return c == '-' || c == '.' ||
    c == 'b' || c == 'x' || c == 'l' || c == 'f' || c == 'p' || c == 'e' ||
    c == 'B' || c == 'X' || c == 'L' || c == 'F' || c == 'P' || c == 'E';
}

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

static buffer_t
foolishly_free_buffer(buffer_t buffer) {
  free(buffer.data);
}

#endif // FOOLISH


static b32_t
meta_tokenizer_init(meta_tokenizer_t* t, const char* filename) {
  t->text = foolishly_read_file_into_buffer(filename, true); 
  t->at = 0;
  return !!t->text.size;
}

static b32_t
meta_compare_token_with_string(meta_tokenizer_t* t, meta_token_t token, st8_t str) {
  if( str.count != (token.ope - token.begin)) {
    return false;
  }

  for(u32_t i = 0; i < str.count; ++i) {
    if (str.e[i] != t->text.data[token.begin+i]) {
      return false;
    }
  }

  return true;
}

static void
meta_print_token(meta_tokenizer_t* t, meta_token_t token)  {
  for(usz_t i = token.begin; i < token.ope; ++i) {
    printf("%c", t->text.data[i]);
  }
}
static b32_t
meta_is_keyword(meta_tokenizer_t* t, meta_token_t token) {
  static st8_t keywords[] = {
    st8_from_lit("if"),
    st8_from_lit("else"),
    st8_from_lit("switch"),
    st8_from_lit("case"),
    st8_from_lit("default"),
    st8_from_lit("while"),
    st8_from_lit("for"),
    st8_from_lit("if"),
    st8_from_lit("operator"),
    st8_from_lit("auto"),
    st8_from_lit("goto"),
  };
  for_arr(i, keywords) {
    if (meta_compare_token_with_string(t, token, keywords[i]))
    {
      return true;
    }
  }
  return false;
}

static void 
meta_tokenizer_free(meta_tokenizer_t* t) {
  free(t->text.data);
}


static meta_token_t
meta_next_token(meta_tokenizer_t* t) {
  meta_eat_ignorables(t);

  meta_token_t ret = {};
  ret.begin = t->at;
  ret.ope = t->at + 1;

  if (t->text.data[t->at] == 0) {
    ret.type = META_TOKEN_TYPE_EOF; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '(') {
    ret.type = META_TOKEN_TYPE_OPEN_PAREN; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '?') {
    ret.type = META_TOKEN_TYPE_QUESTION; 
    ++t->at;
  }
  else if (t->text.data[t->at] == ')') {
    ret.type = META_TOKEN_TYPE_CLOSE_PAREN; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '[') {
    ret.type = META_TOKEN_TYPE_OPEN_BRACKET; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ']') {
    ret.type = META_TOKEN_TYPE_CLOSE_BRACKET; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '{') {
    ret.type = META_TOKEN_TYPE_OPEN_BRACE; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == '}') {
    ret.type = META_TOKEN_TYPE_CLOSE_BRACE; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ')') { 
    ret.type = META_TOKEN_TYPE_COLON; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ';') {
    ret.type = META_TOKEN_TYPE_SEMICOLON; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '+') {
    ret.type = META_TOKEN_TYPE_PLUS;
    ++t->at;
    if (t->text.data[t->at] == '+') { // ++
      ret.type = META_TOKEN_TYPE_PLUS_PLUS;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // +=
      ret.type = META_TOKEN_TYPE_PLUS_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '-') {
    ret.type = META_TOKEN_TYPE_MINUS;
    ++t->at;
    if (t->text.data[t->at] == '-') { // --
      ret.type = META_TOKEN_TYPE_MINUS_MINUS;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // -=
      ret.type = META_TOKEN_TYPE_MINUS_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '>') { // ->
      ret.type = META_TOKEN_TYPE_ARROW;
      ret.ope = ++t->at;
    }
    else if (is_digit(t->text.data[t->at])) // negative number related literals
    {    
      ret.type = META_TOKEN_TYPE_NUMBER;
      while(is_digit(t->text.data[t->at]) ||
            meta_is_accepted_character_for_number(t->text.data[t->at]))
      {
        ++t->at;
      }
      ret.ope = t->at;
    }
  }
  else if (t->text.data[t->at] == '=') {
    ret.type = META_TOKEN_TYPE_EQUAL;
    ++t->at;

    if (t->text.data[t->at] == '=') { // ==
      ret.type = META_TOKEN_TYPE_EQUAL_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '>') {
    ret.type = META_TOKEN_TYPE_GREATER;
    ++t->at;

    if (t->text.data[t->at] == '=') { // >=
      ret.type = META_TOKEN_TYPE_GREATER_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '>') { // >>
      ret.type = META_TOKEN_TYPE_GREATER_GREATER;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '<') {
    ret.type = META_TOKEN_TYPE_LESSER;
    ++t->at;

    if (t->text.data[t->at] == '=') { // >=
      ret.type = META_TOKEN_TYPE_LESSER_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '<') { // <<
      ret.type = META_TOKEN_TYPE_LESSER_LESSER;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '|') {
    ret.type = META_TOKEN_TYPE_OR;
    ++t->at;

    if (t->text.data[t->at] == '|') { // ||
      ret.type = META_TOKEN_TYPE_OR_OR;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // |=
      ret.type = META_TOKEN_TYPE_OR_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == ':') {
    ret.type = META_TOKEN_TYPE_COLON;
    ++t->at;

    if (t->text.data[t->at] == ':') { // ::
      ret.type = META_TOKEN_TYPE_SCOPE;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '&') {
    ret.type = META_TOKEN_TYPE_AND;
    ++t->at;

    if (t->text.data[t->at] == '&') { // &&
      ret.type = META_TOKEN_TYPE_AND_AND;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // &=
      ret.type = META_TOKEN_TYPE_AND_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '*') {
    ret.type = META_TOKEN_TYPE_STAR;
    ++t->at;

    if (t->text.data[t->at] == '=') { // *=
      ret.type = META_TOKEN_TYPE_STAR_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '/') {
    ret.type = META_TOKEN_TYPE_SLASH;
    ++t->at;

    if (t->text.data[t->at] == '=') { // /=
      ret.type = META_TOKEN_TYPE_SLASH_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '%') {
    ret.type = META_TOKEN_TYPE_PERCENT;
    ++t->at;

    if (t->text.data[t->at] == '=') { // %=
      ret.type = META_TOKEN_TYPE_PERCENT_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '^') {
    ret.type = META_TOKEN_TYPE_XOR;
    ++t->at;
    if (t->text.data[t->at] == '=') { // ^=
      ret.type = META_TOKEN_TYPE_XOR_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '~') {
    ret.type = META_TOKEN_TYPE_BITWISE_NOT;
    ++t->at;
  }
  else if (t->text.data[t->at] == '!') {
    ret.type = META_TOKEN_TYPE_LOGICAL_NOT;
    ++t->at;
  }
  else if (t->text.data[t->at] == '.')
  {
    ret.type = META_TOKEN_TYPE_DOT; 
    ++t->at;

    if (is_digit(t->text.data[t->at])) // positive number related literals
    {    
      ret.type = META_TOKEN_TYPE_NUMBER;
      while(is_digit(t->text.data[t->at]) ||
            meta_is_accepted_character_for_number(t->text.data[t->at]))
      {
        ++t->at;
      }
      ret.ope = t->at;
    }
  }

  else if (t->text.data[t->at] == '#') {
    b32_t continue_to_next_line = false;

    ret.type = META_TOKEN_TYPE_MACRO;
    ++t->at;
    while(t->text.data[t->at] != 0) 
    {

      if (t->text.data[t->at] == '\\') {
        continue_to_next_line = true;
      }

      if (t->text.data[t->at] == '\n') 
      {
        if (continue_to_next_line) {
          continue_to_next_line = false;
        }
        else {
          break;
        }
      }

      ++t->at;
    }
    ret.ope = t->at;
  }
  else if (t->text.data[t->at] == '"') // string literals
  {
    ++t->at;
    ret.begin = t->at;
    while(t->text.data[t->at] != '"') 
    {
      if(t->text.data[t->at] == '\\' && 
          t->text.data[t->at+1]) 
      {
        ++t->at;
      }
      ++t->at;
    }
    ret.type = META_TOKEN_TYPE_STRING;
    ret.ope = t->at;
    ++t->at;
  }

  else if (is_alpha(t->text.data[t->at]) || t->text.data[t->at] == '_') 
  {
    while(is_alpha(t->text.data[t->at]) ||
          is_digit(t->text.data[t->at]) ||
          t->text.data[t->at] == '_') 
    {
      ++t->at;
    }
    ret.ope = t->at;
    
    if (meta_is_keyword(t, ret)) {
      ret.type = META_TOKEN_TYPE_KEYWORD;
    }

    else {
      ret.type = META_TOKEN_TYPE_IDENTIFIER;
    } 
  }

  else if (is_digit(t->text.data[t->at])) // positive number related literals
  {    
    ret.type = META_TOKEN_TYPE_NUMBER;
    while(is_digit(t->text.data[t->at]) ||
          meta_is_accepted_character_for_number(t->text.data[t->at]))
    {
      ++t->at;
    }
    ret.ope = t->at;
  }

  else if (t->text.data[t->at] == '\'') // char literals
  {
    ++t->at;
    ret.begin = t->at;
    while(t->text.data[t->at] != '\'') {
      ++t->at;
    }
    ret.type = META_TOKEN_TYPE_CHAR;
    ret.ope = t->at;
    ++t->at;
  }

  else {
    ret.type = META_TOKEN_TYPE_UNKNOWN;
    ++t->at;
  }


  return ret;
}



static void 
meta_is_function_approaching(meta_tokenizer_t* t) {
  meta_tokenizer_t state = (*t);
  meta_token_t token_a =  meta_next_token(t);
  meta_token_t token_b =  meta_next_token(t);
  meta_token_t token_c =  meta_next_token(t);
  if (token_a.type == META_TOKEN_TYPE_IDENTIFIER &&
      token_b.type == META_TOKEN_TYPE_IDENTIFIER &&
      token_c.type == META_TOKEN_TYPE_OPEN_PAREN)
  {
#if 0
    printf("Maybe we are a function: ");
    printf("\n  ");
    meta_print_token(t, token_a);
    printf("\n  ");
    meta_print_token(t, token_b);
    printf("\n  ");
    meta_print_token(t, token_c);
    printf("\n");
    printf("A function is approaching: ");
    meta_print_token(t, token_b);
    printf("\n");
#endif

  }

  // 
  (*t) = state;

}

// In this program
int main() {
  make(meta_tokenizer_t, t);
#if 1
  if (!meta_tokenizer_init(t, "../code/momo.h")){
#else
  if (!meta_tokenizer_init(t, "test.h")){
#endif
    printf("Cannot open file\n");
    return 1;
  }
  defer { meta_tokenizer_free(t); };

  for(;;) {
    meta_token_t token = meta_next_token(t);
#if 1
    printf("Type: %d\n", token.type);
    meta_print_token(t, token);
    printf("\n===\n");
#endif

    if (token.type == META_TOKEN_TYPE_EOF) 
      break;
  } 
}

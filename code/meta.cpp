// 
// TODO: parse negative numbers correctly
//

#include <stdio.h>
#include "momo.h"

enum clex_token_type_t {
  CLEX_TOKEN_TYPE_UNKNOWN,
  CLEX_TOKEN_TYPE_OPEN_PAREN,          // (
  CLEX_TOKEN_TYPE_CLOSE_PAREN,         // )
  CLEX_TOKEN_TYPE_SEMICOLON,           // ;
  CLEX_TOKEN_TYPE_COMMA,               // ,
  CLEX_TOKEN_TYPE_DOT,                 // ,
  CLEX_TOKEN_TYPE_COLON,               // : 
  CLEX_TOKEN_TYPE_SCOPE,               // :: 
  CLEX_TOKEN_TYPE_OPEN_BRACKET,        // [
  CLEX_TOKEN_TYPE_CLOSE_BRACKET,       // ]
  CLEX_TOKEN_TYPE_OPEN_BRACE,          // {
  CLEX_TOKEN_TYPE_CLOSE_BRACE,         // } 
  CLEX_TOKEN_TYPE_PLUS,                // +
  CLEX_TOKEN_TYPE_PLUS_PLUS,           // ++ 
  CLEX_TOKEN_TYPE_PLUS_EQUAL,          // += 
  CLEX_TOKEN_TYPE_MINUS,               // -
  CLEX_TOKEN_TYPE_MINUS_MINUS,         // -- 
  CLEX_TOKEN_TYPE_MINUS_EQUAL,         // -= 
  CLEX_TOKEN_TYPE_ARROW,               // ->
  CLEX_TOKEN_TYPE_EQUAL,               // =
  CLEX_TOKEN_TYPE_EQUAL_EQUAL,         // ==
  CLEX_TOKEN_TYPE_GREATER,             // > 
  CLEX_TOKEN_TYPE_GREATER_EQUAL,       // >= 
  CLEX_TOKEN_TYPE_GREATER_GREATER,     // >>
  CLEX_TOKEN_TYPE_LESSER,              // < 
  CLEX_TOKEN_TYPE_LESSER_EQUAL,        // <= 
  CLEX_TOKEN_TYPE_LESSER_LESSER,       // <<
  CLEX_TOKEN_TYPE_OR,                  // |
  CLEX_TOKEN_TYPE_OR_EQUAL,            // |=
  CLEX_TOKEN_TYPE_OR_OR,               // ||
  CLEX_TOKEN_TYPE_AND,                 // & 
  CLEX_TOKEN_TYPE_AND_AND,             // &&
  CLEX_TOKEN_TYPE_AND_EQUAL,           // &=
  CLEX_TOKEN_TYPE_LOGICAL_NOT,         // !
  CLEX_TOKEN_TYPE_BITWISE_NOT,         // ~
  CLEX_TOKEN_TYPE_XOR,                 // ^
  CLEX_TOKEN_TYPE_XOR_EQUAL,           // ^=
  CLEX_TOKEN_TYPE_QUESTION,            // ?
  CLEX_TOKEN_TYPE_STAR,                // *
  CLEX_TOKEN_TYPE_STAR_EQUAL,          // *=
  CLEX_TOKEN_TYPE_SLASH,               // /
  CLEX_TOKEN_TYPE_SLASH_EQUAL,         // /=
  CLEX_TOKEN_TYPE_PERCENT,             // %
  CLEX_TOKEN_TYPE_PERCENT_EQUAL,       // %=
  CLEX_TOKEN_TYPE_IDENTIFIER,
  CLEX_TOKEN_TYPE_KEYWORD,
  CLEX_TOKEN_TYPE_STRING,
  CLEX_TOKEN_TYPE_NUMBER,
  CLEX_TOKEN_TYPE_CHAR,
  CLEX_TOKEN_TYPE_MACRO,
  CLEX_TOKEN_TYPE_EOF
};

struct clex_token_t {
  clex_token_type_t type;

  usz_t begin;
  usz_t ope;
};

struct clex_tokenizer_t {
  buffer_t text;
  usz_t at;
};


static void
_clex_eat_ignorables(clex_tokenizer_t* t) {
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
clex_is_accepted_character_for_number(char c) {
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

static void
foolishly_free_buffer(buffer_t buffer) {
  free(buffer.data);
}

#endif // FOOLISH


static b32_t
clex_tokenizer_init(clex_tokenizer_t* t, buffer_t buffer) {
  t->text = buffer; 
  t->at = 0;
  return !!t->text.size;
}

static b32_t
clex_compare_token_with_string(clex_tokenizer_t* t, clex_token_t token, st8_t str) {
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
meta_print_token(clex_tokenizer_t* t, clex_token_t token)  {
  for(usz_t i = token.begin; i < token.ope; ++i) {
    putchar(t->text.data[i]);
  }
}

static b32_t
_clex_is_keyword(clex_tokenizer_t* t, clex_token_t token) {
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
    st8_from_lit("return"),
  };
  for_arr(i, keywords) {
    if (clex_compare_token_with_string(t, token, keywords[i]))
    {
      return true;
    }
  }
  return false;
}


static clex_token_t
clex_next_token(clex_tokenizer_t* t) {
  _clex_eat_ignorables(t);

  clex_token_t ret = {};
  ret.begin = t->at;
  ret.ope = t->at + 1;

  if (t->text.data[t->at] == 0) {
    ret.type = CLEX_TOKEN_TYPE_EOF; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '(') {
    ret.type = CLEX_TOKEN_TYPE_OPEN_PAREN; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '?') {
    ret.type = CLEX_TOKEN_TYPE_QUESTION; 
    ++t->at;
  }
  else if (t->text.data[t->at] == ')') {
    ret.type = CLEX_TOKEN_TYPE_CLOSE_PAREN; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '[') {
    ret.type = CLEX_TOKEN_TYPE_OPEN_BRACKET; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ']') {
    ret.type = CLEX_TOKEN_TYPE_CLOSE_BRACKET; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '{') {
    ret.type = CLEX_TOKEN_TYPE_OPEN_BRACE; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == '}') {
    ret.type = CLEX_TOKEN_TYPE_CLOSE_BRACE; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ')') { 
    ret.type = CLEX_TOKEN_TYPE_COLON; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ';') {
    ret.type = CLEX_TOKEN_TYPE_SEMICOLON; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '+') {
    ret.type = CLEX_TOKEN_TYPE_PLUS;
    ++t->at;
    if (t->text.data[t->at] == '+') { // ++
      ret.type = CLEX_TOKEN_TYPE_PLUS_PLUS;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // +=
      ret.type = CLEX_TOKEN_TYPE_PLUS_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '-') {
    ret.type = CLEX_TOKEN_TYPE_MINUS;
    ++t->at;
    if (t->text.data[t->at] == '-') { // --
      ret.type = CLEX_TOKEN_TYPE_MINUS_MINUS;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // -=
      ret.type = CLEX_TOKEN_TYPE_MINUS_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '>') { // ->
      ret.type = CLEX_TOKEN_TYPE_ARROW;
      ret.ope = ++t->at;
    }
    else if (is_digit(t->text.data[t->at])) // negative number related literals
    {    
      ret.type = CLEX_TOKEN_TYPE_NUMBER;
      while(is_digit(t->text.data[t->at]) ||
            clex_is_accepted_character_for_number(t->text.data[t->at]))
      {
        ++t->at;
      }
      ret.ope = t->at;
    }
  }
  else if (t->text.data[t->at] == '=') {
    ret.type = CLEX_TOKEN_TYPE_EQUAL;
    ++t->at;

    if (t->text.data[t->at] == '=') { // ==
      ret.type = CLEX_TOKEN_TYPE_EQUAL_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '>') {
    ret.type = CLEX_TOKEN_TYPE_GREATER;
    ++t->at;

    if (t->text.data[t->at] == '=') { // >=
      ret.type = CLEX_TOKEN_TYPE_GREATER_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '>') { // >>
      ret.type = CLEX_TOKEN_TYPE_GREATER_GREATER;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '<') {
    ret.type = CLEX_TOKEN_TYPE_LESSER;
    ++t->at;

    if (t->text.data[t->at] == '=') { // >=
      ret.type = CLEX_TOKEN_TYPE_LESSER_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '<') { // <<
      ret.type = CLEX_TOKEN_TYPE_LESSER_LESSER;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '|') {
    ret.type = CLEX_TOKEN_TYPE_OR;
    ++t->at;

    if (t->text.data[t->at] == '|') { // ||
      ret.type = CLEX_TOKEN_TYPE_OR_OR;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // |=
      ret.type = CLEX_TOKEN_TYPE_OR_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == ':') {
    ret.type = CLEX_TOKEN_TYPE_COLON;
    ++t->at;

    if (t->text.data[t->at] == ':') { // ::
      ret.type = CLEX_TOKEN_TYPE_SCOPE;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '&') {
    ret.type = CLEX_TOKEN_TYPE_AND;
    ++t->at;

    if (t->text.data[t->at] == '&') { // &&
      ret.type = CLEX_TOKEN_TYPE_AND_AND;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // &=
      ret.type = CLEX_TOKEN_TYPE_AND_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '*') {
    ret.type = CLEX_TOKEN_TYPE_STAR;
    ++t->at;

    if (t->text.data[t->at] == '=') { // *=
      ret.type = CLEX_TOKEN_TYPE_STAR_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '/') {
    ret.type = CLEX_TOKEN_TYPE_SLASH;
    ++t->at;

    if (t->text.data[t->at] == '=') { // /=
      ret.type = CLEX_TOKEN_TYPE_SLASH_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '%') {
    ret.type = CLEX_TOKEN_TYPE_PERCENT;
    ++t->at;

    if (t->text.data[t->at] == '=') { // %=
      ret.type = CLEX_TOKEN_TYPE_PERCENT_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '^') {
    ret.type = CLEX_TOKEN_TYPE_XOR;
    ++t->at;
    if (t->text.data[t->at] == '=') { // ^=
      ret.type = CLEX_TOKEN_TYPE_XOR_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '~') {
    ret.type = CLEX_TOKEN_TYPE_BITWISE_NOT;
    ++t->at;
  }
  else if (t->text.data[t->at] == '!') {
    ret.type = CLEX_TOKEN_TYPE_LOGICAL_NOT;
    ++t->at;
  }
  else if (t->text.data[t->at] == '.')
  {
    ret.type = CLEX_TOKEN_TYPE_DOT; 
    ++t->at;

    if (is_digit(t->text.data[t->at])) // positive number related literals
    {    
      ret.type = CLEX_TOKEN_TYPE_NUMBER;
      while(is_digit(t->text.data[t->at]) ||
            clex_is_accepted_character_for_number(t->text.data[t->at]))
      {
        ++t->at;
      }
      ret.ope = t->at;
    }
  }

  else if (t->text.data[t->at] == '#') {
    b32_t continue_to_next_line = false;

    ret.type = CLEX_TOKEN_TYPE_MACRO;
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
    ret.type = CLEX_TOKEN_TYPE_STRING;
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
    
    if (_clex_is_keyword(t, ret)) {
      ret.type = CLEX_TOKEN_TYPE_KEYWORD;
    }

    else {
      ret.type = CLEX_TOKEN_TYPE_IDENTIFIER;
    } 
  }

  else if (is_digit(t->text.data[t->at])) // positive number related literals
  {    
    ret.type = CLEX_TOKEN_TYPE_NUMBER;
    while(is_digit(t->text.data[t->at]) ||
          clex_is_accepted_character_for_number(t->text.data[t->at]))
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
    ret.type = CLEX_TOKEN_TYPE_CHAR;
    ret.ope = t->at;
    ++t->at;
  }

  else {
    ret.type = CLEX_TOKEN_TYPE_UNKNOWN;
    ++t->at;
  }


  return ret;
}



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

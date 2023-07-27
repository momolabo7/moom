#include <stdio.h>
#include "momo.h"

enum meta_token_type_t {
  META_TOKEN_TYPE_UNKNOWN,
  META_TOKEN_TYPE_OPEN_PAREN,
  META_TOKEN_TYPE_CLOSE_PAREN,
  META_TOKEN_TYPE_SEMICOLON,
  META_TOKEN_TYPE_COLON,
  META_TOKEN_TYPE_OPEN_BRACKET,
  META_TOKEN_TYPE_CLOSE_BRACKET,
  META_TOKEN_TYPE_OPEN_BRACE,
  META_TOKEN_TYPE_CLOSE_BRACE,


  META_TOKEN_TYPE_IDENTIFIER,
  META_TOKEN_TYPE_KEYWORD,
  META_TOKEN_TYPE_STRING,
  META_TOKEN_TYPE_NUMBER,
  META_TOKEN_TYPE_MACRO,
  META_TOKEN_TYPE_OPERATOR,

  META_TOKEN_TYPE_EOF
};

struct meta_token_t {
  meta_token_type_t type;

  u32_t begin;
  u32_t ope;
};

struct meta_tokenizer_t {
  c8_t* text;
  u32_t at;
  u32_t text_length;
};

static c8_t
meta_current_char(meta_tokenizer_t* t) {
  return t->text[t->at];
}

static void
meta_advance(meta_tokenizer_t* t) {
  t->at++;
}

static void
meta_eat_ignorables(meta_tokenizer_t* t) {
  for (;;) {
    if(is_whitespace(t->text[t->at])) {
      meta_advance(t);
    }
    else if(t->text[t->at] == '/' && t->text[t->at+1] == '/') {
      t->at += 3;
      while(t->text[t->at] != '\n' && t->text[t->at] != '\r') {
        meta_advance(t);
      }
    }
    else if(t->text[t->at] == '/' && t->text[t->at+1] == '*') {
      t->at += 3;
    }
    else {
      break;
    }
  }

}

static b32_t
meta_tokenizer_init(meta_tokenizer_t* t, const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (fp) {
    fseek(fp, 1, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 1, SEEK_SET);
    void* ptr = malloc(size + 1);
    fread(ptr, size, 1, fp);
    fclose(fp);

    t->text = (c8_t*)ptr;
    t->text_length = size + 1;

    // nullptr terminate
    t->text[size] = 0;

    return true;
  }
  return false;
}

static b32_t
meta_compare_token_with_string(meta_tokenizer_t* t, meta_token_t token, str8_t str) {
  if( str.count != (token.ope - token.begin)) {
    return false;
  }

  for(u32_t i = 0; i < str.count; ++i) {
    if (str.e[i] != t->text[token.begin+i]) {
      return false;
    }
  }

  return true;
}

static b32_t
meta_is_keyword(meta_tokenizer_t* t, meta_token_t token) {
  static str8_t keywords[] = {
    str8_from_lit("if"),
    str8_from_lit("else"),
    str8_from_lit("switch"),
    str8_from_lit("case"),
    str8_from_lit("default"),
    str8_from_lit("while"),
    str8_from_lit("for"),
    str8_from_lit("if"),
    str8_from_lit("operator"),
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
  free(t->text);
}


static meta_token_t
meta_next_token(meta_tokenizer_t* t) {
  meta_eat_ignorables(t);

  meta_token_t ret = {};
  ret.begin = t->at;
  ret.ope = t->at + 1;

  if (meta_current_char(t) == 0) {
    ret.type = META_TOKEN_TYPE_EOF; 
    meta_advance(t);
  }
  else if (meta_current_char(t) == '(') {
    ret.type = META_TOKEN_TYPE_OPEN_PAREN; 
    meta_advance(t);
  }
  else if (meta_current_char(t) == ')') {
    ret.type = META_TOKEN_TYPE_CLOSE_PAREN; 
    meta_advance(t);
  }
  else if (meta_current_char(t) == '[') {
    ret.type = META_TOKEN_TYPE_OPEN_BRACKET; 
    meta_advance(t);
  } 
  else if (meta_current_char(t) == ']') {
    ret.type = META_TOKEN_TYPE_CLOSE_BRACKET; 
    meta_advance(t);
  }
  else if (meta_current_char(t) == '{') {
    ret.type = META_TOKEN_TYPE_OPEN_BRACE; 
    meta_advance(t);
  } 
  else if (meta_current_char(t) == '}') {
    ret.type = META_TOKEN_TYPE_CLOSE_BRACE; 
    meta_advance(t);
  } 
  else if (meta_current_char(t) == ')') { 
    ret.type = META_TOKEN_TYPE_COLON; 
    meta_advance(t);
  } 
  else if (meta_current_char(t) == ';') {
    ret.type = META_TOKEN_TYPE_SEMICOLON; 
    meta_advance(t);
  }
  else if (meta_current_char(t) == '+' || 
           meta_current_char(t) == '-' ||
           meta_current_char(t) == '*' ||
           meta_current_char(t) == '/' ||
           meta_current_char(t) == '%' ||
           meta_current_char(t) == ',' ||
           meta_current_char(t) == '.')
  {
    ret.type = META_TOKEN_TYPE_OPERATOR; 
    meta_advance(t);
  }

  else if (meta_current_char(t) == '#') {
    ret.type = META_TOKEN_TYPE_MACRO;
    // ignore the whole line
    // TODO: include multi line macros
    b32_t continues_to_next_line = false;
    while(t->text[t->at] != '\n' && t->text[t->at] != '\r') {
      meta_advance(t);
    }
    ret.ope = t->at;
  }
  else if (meta_current_char(t) == '"') // strings
  {
    meta_advance(t);
    ret.begin = t->at;
    while(t->text[t->at] &&
        t->text[t->at] != '"') 
    {
      if(t->text[t->at] == '\\' && 
          t->text[t->at+1]) 
      {
        meta_advance(t);
      }
      meta_advance(t);
    }
    ret.type = META_TOKEN_TYPE_STRING;
    ret.ope = t->at;
    meta_advance(t);
  }

  else if (is_alpha(meta_current_char(t)) || meta_current_char(t) == '_') 
  {
    while(is_alpha(meta_current_char(t)) ||
          is_digit(meta_current_char(t)) ||
          meta_current_char(t) == '_') 
    {
      meta_advance(t);
    }
    ret.ope = t->at;
    
    if (meta_is_keyword(t, ret)) {
      ret.type = META_TOKEN_TYPE_KEYWORD;
    }

    else {
      ret.type = META_TOKEN_TYPE_IDENTIFIER;
    } 
  }

  else {
    ret.type = META_TOKEN_TYPE_UNKNOWN;
    meta_advance(t);
  }


  return ret;
}

static void
meta_print_token(meta_tokenizer_t* t, meta_token_t token)  {
  switch(token.type) {
    case META_TOKEN_TYPE_OPEN_PAREN: 
    case META_TOKEN_TYPE_CLOSE_PAREN:
    case META_TOKEN_TYPE_SEMICOLON:
    case META_TOKEN_TYPE_COLON:
    case META_TOKEN_TYPE_OPEN_BRACKET:
    case META_TOKEN_TYPE_CLOSE_BRACKET:
    case META_TOKEN_TYPE_OPEN_BRACE:
    case META_TOKEN_TYPE_CLOSE_BRACE:
    case META_TOKEN_TYPE_OPERATOR:
    {
      printf("token: ");
    } break;
    case META_TOKEN_TYPE_IDENTIFIER: {
      printf("identifier: ");
    } break;
    case META_TOKEN_TYPE_STRING: {
      printf("string: ");
    } break;
    case META_TOKEN_TYPE_EOF: {
      printf("eof");
    } break;
    case META_TOKEN_TYPE_UNKNOWN: {
      printf("unknown: ");
    } break;
    case META_TOKEN_TYPE_MACRO: {
      printf("macro: ");
    } break;
  }
  for(u32_t i = token.begin; i < token.ope; ++i) {
    printf("%c", t->text[i]);
  }
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
#else
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
  if (!meta_tokenizer_init(t, "../code/momo2.h")){
    printf("Cannot open file\n");
    return 1;
  }
  defer { meta_tokenizer_free(t); };

  for(;;) {
    meta_is_function_approaching(t);

    meta_token_t token = meta_next_token(t);
#if 0
    meta_print_token(t, token);
    printf("\n");
#endif

    if (token.type == META_TOKEN_TYPE_EOF) 
      break;
  } 
}

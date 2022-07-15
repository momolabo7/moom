#include <stdio.h>
#include "momo.h"

enum Meta_Token_Type {
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
  META_TOKEN_TYPE_STRING,
  META_TOKEN_TYPE_NUMBER,
  META_TOKEN_TYPE_MACRO,
  
  META_TOKEN_TYPE_EOF
};

struct Meta_Token {
  Meta_Token_Type type;
  
  U32 begin;
  U32 ope;
};

struct Meta_Tokenizer {
  C8* text;
  U32 at;
  U32 text_length;
};

static void
meta_eat_ignorables(Meta_Tokenizer* t) {
  for (;;) {
    if(is_whitespace(t->text[t->at])) {
      ++t->at;
    }
    else if(t->text[t->at] == '/' && t->text[t->at+1] == '/') {
      t->at += 2;
      while(t->text[t->at] != '\n' && t->text[t->at] != '\r') {
        ++t->at;
      }
    }
    else if(t->text[t->at] == '/' && t->text[t->at+1] == '*') {
      t->at += 2;
      
    }
    else {
      break;
    }
  }
  
}

static B32
meta_tokenizer_init(Meta_Tokenizer* t, const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (fp) {
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    void* ptr = malloc(size + 1);
    fread(ptr, size, 1, fp);
    fclose(fp);
    
    t->text = (C8*)ptr;
    t->text_length = size + 1;
    
    // null terminate
    t->text[size] = 0;
    
    return true;
  }
  return false;
}

static void 
meta_tokenizer_free(Meta_Tokenizer* t) {
  free(t->text);
}


static Meta_Token
meta_next_token(Meta_Tokenizer* t) {
  meta_eat_ignorables(t);
  
  Meta_Token ret = {};
  ret.begin = t->at;
  ret.ope = t->at + 1;
  
  
  switch(t->text[t->at]) {
    case '\0': {
      ret.type = META_TOKEN_TYPE_EOF; 
      ++t->at;
    } break;
    
    case '(': {
      ret.type = META_TOKEN_TYPE_OPEN_PAREN; 
      ++t->at;
    } break;
    case ')': {
      ret.type = META_TOKEN_TYPE_CLOSE_PAREN; 
      ++t->at;
    } break;
    case '[': {
      ret.type = META_TOKEN_TYPE_OPEN_BRACKET; 
    } break;
    case ']': {
      ret.type = META_TOKEN_TYPE_CLOSE_BRACKET; 
      ++t->at;
    } break;
    case '{': {
      ret.type = META_TOKEN_TYPE_OPEN_BRACE; 
      ++t->at;
    } break;
    case '}': {
      ret.type = META_TOKEN_TYPE_CLOSE_BRACE; 
      ++t->at;
    } break;
    case ':': { 
      ret.type = META_TOKEN_TYPE_COLON; 
      ++t->at;
    } break;
    case ';': {
      ret.type = META_TOKEN_TYPE_SEMICOLON; 
      ++t->at;
    } break;
    case '#': {
      ret.type = META_TOKEN_TYPE_MACRO;
      //ignore the whole line
      while(t->text[t->at] != '\n' && t->text[t->at] != '\r') {
        ++t->at;
      }
      ret.ope = t->at;
    } break;
    case '"': // strings
    {
      ++t->at;
      ret.begin = t->at;
      while(t->text[t->at] &&
            t->text[t->at] != '"') 
      {
        if(t->text[t->at] == '\\' && 
           t->text[t->at+1]) 
        {
          ++t->at;
        }
        ++t->at;
      }
      ret.type = META_TOKEN_TYPE_STRING;
      ret.ope = t->at;
      ++t->at;
    } break;
    
    default: {
      ret.type = META_TOKEN_TYPE_IDENTIFIER;
      
      if (is_alpha(t->text[t->at]) || t->text[t->at] == '_') {
        while(is_alpha(t->text[t->at]) ||
              is_digit(t->text[t->at]) ||
              t->text[t->at] == '_') 
        {
          ++t->at;
        }
        ret.ope = t->at;
        ret.type = META_TOKEN_TYPE_IDENTIFIER;
        
      }
#if 0
      else if (is_digit(t->text[t->at])) {
        // TODO:
        // - binary
        // - hexa
        // - normal
        // - float
        // - double
        // others?
      }
#endif
    }
    
  }
  return ret;
}

static void
meta_print_token(Meta_Tokenizer* t, Meta_Token token)  {
  switch(token.type) {
    case META_TOKEN_TYPE_OPEN_PAREN: 
    case META_TOKEN_TYPE_CLOSE_PAREN:
    case META_TOKEN_TYPE_SEMICOLON:
    case META_TOKEN_TYPE_COLON:
    case META_TOKEN_TYPE_OPEN_BRACKET:
    case META_TOKEN_TYPE_CLOSE_BRACKET:
    case META_TOKEN_TYPE_OPEN_BRACE:
    case META_TOKEN_TYPE_CLOSE_BRACE:{
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
  for(U32 i = token.begin; i < token.ope; ++i) {
    printf("%c", t->text[i]);
  }
}

static B32
meta_compare_token_with_string(Meta_Tokenizer* t, Meta_Token token, String str) {
  if( str.count != (token.ope - token.begin)) {
    return false;
  }
  
  for(U32 i = 0; i < str.count; ++i) {
    if (str.e[i] != t->text[token.begin+i]) {
      return false;
    }
  }
  
  return true;
}

struct Profiler_Codegen {
  U32 state; // 
  U32 units;
};

static void
meta_update_profiler_codegen(Profiler_Codegen* p, Meta_Tokenizer* t, Meta_Token token) {
  switch(p->state) {
    case 0: {
      if (meta_compare_token_with_string(t, token, string_from_lit("wtf_sia"))) {
        printf(" <-");
        ++p->state;
      }  
    } break; 
    case 1: {
      if (token.type == META_TOKEN_TYPE_OPEN_PAREN) ++p->state;
    } break;
    case 2: {
      // grab the next identifier
      if (token.type == META_TOKEN_TYPE_IDENTIFIER) {
        printf(" <!");
        p->state = 0;
        ++p->units;
      }
    } 
  }
}


int main() {
  declare_and_pointerize(Meta_Tokenizer, t);
  if (!meta_tokenizer_init(t, "meta_test.cpp")){
    printf("Cannot open file\n");
    return 1;
  }
  defer { meta_tokenizer_free(t); };
  
  declare_and_pointerize(Profiler_Codegen, p);
  
  for(;;) {
    Meta_Token token = meta_next_token(t);
    meta_print_token(t, token);
    meta_update_profiler_codegen(p, t, token);
    
    printf("\n");
    if (token.type == META_TOKEN_TYPE_EOF) 
      break;
  } 
  
  printf("number of profiler units: %d\n", p->units);
}

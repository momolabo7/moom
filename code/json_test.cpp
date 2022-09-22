#include <stdio.h>
#include <stdlib.h>

#include "momo.h"

typedef enum  {
  JSON_TOKEN_TYPE_UNKNOWN,
  JSON_TOKEN_TYPE_OPEN_PAREN,
  JSON_TOKEN_TYPE_CLOSE_PAREN,
  JSON_TOKEN_TYPE_SEMICOLON,
  JSON_TOKEN_TYPE_COLON,
  JSON_TOKEN_TYPE_OPEN_BRACKET,
  JSON_TOKEN_TYPE_CLOSE_BRACKET,
  JSON_TOKEN_TYPE_OPEN_BRACE,
  JSON_TOKEN_TYPE_CLOSE_BRACE,
  JSON_TOKEN_TYPE_COMMA,
  
  
  JSON_TOKEN_TYPE_IDENTIFIER,
  JSON_TOKEN_TYPE_STRING,
  JSON_TOKEN_TYPE_NUMBER,
  
  JSON_TOKEN_TYPE_EOF
}Json_Token_Type;

typedef struct {
  Json_Token_Type type;
  
  U32 begin;
  U32 ope;
}Json_Token;

typedef struct  {
  C8* text;
  U32 at;
  U32 text_len;
} Json_Tokenizer;

static Json_Tokenizer 
json_read_for_tokenizer(const C8* filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp) { 
    printf("Cannot open file\n");
    return {0};
  }
  fseek(fp, 0, SEEK_END);
  long len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  void* mem = malloc(len);
  if (!mem) {
    printf("Cannot allocate memory\n");
    return {0};
  }
  fread(mem, len, 1, fp);
  fclose(fp);

  Json_Tokenizer ret = {0};
  ret.text = (C8*)mem;
  ret.text_len = len;

  return ret;
}


static void
json_eat_ignorables(Json_Tokenizer* t) {
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
static Json_Token
json_next_token(Json_Tokenizer* t) {
  json_eat_ignorables(t);
  
  Json_Token ret = {};
  ret.begin = t->at;
  ret.ope = t->at + 1;
  
  
  switch(t->text[t->at]) {
    case '\0': {
      ret.type = JSON_TOKEN_TYPE_EOF; 
      ++t->at;
    } break;
    
    case '(': {
      ret.type = JSON_TOKEN_TYPE_OPEN_PAREN; 
      ++t->at;
    } break;
    case ')': {
      ret.type = JSON_TOKEN_TYPE_CLOSE_PAREN; 
      ++t->at;
    } break;
    case '[': {
      ret.type = JSON_TOKEN_TYPE_OPEN_BRACKET; 
    } break;
    case ']': {
      ret.type = JSON_TOKEN_TYPE_CLOSE_BRACKET; 
      ++t->at;
    } break;
    case '{': {
      ret.type = JSON_TOKEN_TYPE_OPEN_BRACE; 
      ++t->at;
    } break;
    case '}': {
      ret.type = JSON_TOKEN_TYPE_CLOSE_BRACE; 
      ++t->at;
    } break;
    case ',': {
      ret.type = JSON_TOKEN_TYPE_COMMA;
      ++t->at;
    } break;
    case ':': { 
      ret.type = JSON_TOKEN_TYPE_COLON; 
      ++t->at;
    } break;
    case ';': {
      ret.type = JSON_TOKEN_TYPE_SEMICOLON; 
      ++t->at;
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
      ret.type = JSON_TOKEN_TYPE_STRING;
      ret.ope = t->at;
      ++t->at;
    } break;
    
    default: {
      ret.type = JSON_TOKEN_TYPE_IDENTIFIER;
      while(is_alpha(t->text[t->at]) ||
            is_digit(t->text[t->at]) ||
            t->text[t->at] == '_') 
      {
        ++t->at;
      }
      ret.ope = t->at;
      ret.type = JSON_TOKEN_TYPE_IDENTIFIER;
    
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
json_print_token(Json_Tokenizer* t, Json_Token token)  {
#if 0
  switch(token.type) {
    case JSON_TOKEN_TYPE_OPEN_PAREN: 
    case JSON_TOKEN_TYPE_CLOSE_PAREN:
    case JSON_TOKEN_TYPE_SEMICOLON:
    case JSON_TOKEN_TYPE_COLON:
    case JSON_TOKEN_TYPE_COMMA:
    case JSON_TOKEN_TYPE_OPEN_BRACKET:
    case JSON_TOKEN_TYPE_CLOSE_BRACKET:
    case JSON_TOKEN_TYPE_OPEN_BRACE:
    case JSON_TOKEN_TYPE_CLOSE_BRACE:{
      printf("token: ");
    } break;
    case JSON_TOKEN_TYPE_IDENTIFIER: {
      printf("identifier: ");
    } break;
    case JSON_TOKEN_TYPE_STRING: {
      printf("string: ");
    } break;
    case JSON_TOKEN_TYPE_EOF: {
      printf("eof");
    } break;
    case JSON_TOKEN_TYPE_UNKNOWN: {
      printf("unknown: ");
    } break;
    
  }
#endif 
  for(U32 i = token.begin; i < token.ope; ++i) {
    printf("%c", t->text[i]);
  }
}
#if 0
typedef struct {
  struct Json_Node* left;
  struct Json_Node* right;
  String8 key;
  String8 value;
} Json_Node;
#endif 


int main() {
  Json_Tokenizer j = json_read_for_tokenizer("test_json.json");
 
  U32 scope_level = 0;
  B32 is_done = false;

  // 0 is key
  // 1 is ':'
  // 2 is value
  // 3 is ','
  U32 what_to_expect = 0; 
  U32 error_no = 0;

  while(!is_done) {
    Json_Token token = json_next_token(&j);
    if ( token.type == JSON_TOKEN_TYPE_OPEN_BRACE ) {
      ++scope_level;
    }
    else if (token.type == JSON_TOKEN_TYPE_CLOSE_BRACE) {
      --scope_level;
      if (scope_level == 0){
        is_done = true;
      }
    }
    else if(token.type == JSON_TOKEN_TYPE_STRING) {
      if (what_to_expect == 0) {
        printf("key: "); 
        json_print_token(&j, token);
        printf("\n");
        what_to_expect = 1;
      }
      else if (what_to_expect == 2) {
        printf("value: "); 
        json_print_token(&j, token);
        printf("\n");
        what_to_expect = 3;  
      }
      else {
        // TODO: error
        is_done = true;
      }
    }
    else if (token.type == JSON_TOKEN_TYPE_IDENTIFIER) {
      if (what_to_expect == 0) {
        printf("key: "); 
        json_print_token(&j, token);
        printf("\n");
        what_to_expect = 1;
      }
      else if (what_to_expect == 2) {
        printf("value: "); 
        json_print_token(&j, token);
        printf("\n");
        what_to_expect = 3;  
      }
      else {
        // TODO: error
        is_done = true;
      }

    }
    else if (token.type == JSON_TOKEN_TYPE_COLON) {
      if (what_to_expect == 1) {
        what_to_expect = 2;
      }
      else {
        // TODO: error
        is_done = true;
      }
    }
    else if (token.type == JSON_TOKEN_TYPE_COMMA) {

      if (what_to_expect == 3) {
        what_to_expect = 0;
      }
      else {
        // TODO: error
        is_done = true;
      }
    }
    else if (token.type == JSON_TOKEN_TYPE_EOF) {
      // we shouldn't reach here?
      // TODO: error
      is_done = true;
    }
    
  } 


}




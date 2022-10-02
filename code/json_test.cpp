#include <stdlib.h>

#include <stdio.h>
#define JSON_DEBUG 1

#include "momo.h"


// TODO: 
//  array 

typedef enum  {
  _JSON_TOKEN_TYPE_UNKNOWN,
  _JSON_TOKEN_TYPE_COLON,
  _JSON_TOKEN_TYPE_OPEN_BRACKET,
  _JSON_TOKEN_TYPE_CLOSE_BRACKET,
  _JSON_TOKEN_TYPE_OPEN_BRACE,
  _JSON_TOKEN_TYPE_CLOSE_BRACE,
  _JSON_TOKEN_TYPE_COMMA,
  _JSON_TOKEN_TYPE_NULL,
  
  _JSON_TOKEN_TYPE_IDENTIFIER,
  _JSON_TOKEN_TYPE_STRING,
  _JSON_TOKEN_TYPE_UNSIGNED_INTEGER,
  _JSON_TOKEN_TYPE_FLOATING_POINT,
  _JSON_TOKEN_TYPE_SIGNED_INTEGER,
  _JSON_TOKEN_TYPE_TRUE,
  _JSON_TOKEN_TYPE_FALSE,
  
  _JSON_TOKEN_TYPE_EOF
} _JSON_Token_Type;

typedef enum  {
  _JSON_VALUE_TYPE_U32,
  _JSON_VALUE_TYPE_S32,
  _JSON_VALUE_TYPE_F32,
  _JSON_VALUE_TYPE_B32,
  _JSON_VALUE_TYPE_STR8,
  _JSON_VALUE_TYPE_ARRAY,
  _JSON_VALUE_TYPE_OBJECT,
  _JSON_VALUE_TYPE_NULL
} _JSON_Value_Type;


typedef enum {
  _JSON_EXPECT_TYPE_OPEN,
  _JSON_EXPECT_TYPE_KEY_OR_CLOSE,
  _JSON_EXPECT_TYPE_VALUE,
  _JSON_EXPECT_TYPE_COMMA_OR_CLOSE,
  _JSON_EXPECT_TYPE_COLON,
} _JSON_Expect_Type;

typedef struct {
  // TODO
} JSON_Array;

typedef struct {
  _JSON_Token_Type type;
  U32 begin;
  U32 ope;
}_JSON_Token;

typedef struct  {
  String8 text;
  U32 at;
}  _JSON_Tokenizer;

typedef struct 
{
  struct _JSON_Node* node;
} JSON;

typedef struct {
  _JSON_Value_Type type;
  union {
    B32 b32;
    U32 u32;
    S32 s32;
    String8 string;
    F32 f32;
    JSON obj;
  };
} _JSON_Value;

typedef struct _JSON_Node {
  String8 key;
  _JSON_Value value;
  struct _JSON_Node* left;
  struct _JSON_Node* right;
    
} _JSON_Node;



static void
_json_eat_ignorables(_JSON_Tokenizer* t) {
  for (;;) {
    if(is_whitespace(t->text.e[t->at])) {
      ++t->at;
    }
    else if(t->text.e[t->at] == '/' && t->text.e[t->at+1] == '/') {
      t->at += 2;
      while(t->text.e[t->at] != '\n' && t->text.e[t->at] != '\r') {
        ++t->at;
      }
    }
    else if(t->text.e[t->at] == '/' && t->text.e[t->at+1] == '*') {
      t->at += 2;
      
    }
    else {
      break;
    }
  }
}

static _JSON_Token
_json_next_token(_JSON_Tokenizer* t) {
  _json_eat_ignorables(t);
  
  _JSON_Token ret = {0};
  ret.begin = t->at;
  ret.ope = t->at + 1;
  ret.type = _JSON_TOKEN_TYPE_UNKNOWN;
  
  switch(t->text.e[t->at]) {
    case '\0': {
      ret.type = _JSON_TOKEN_TYPE_EOF; 
      ++t->at;
    } break;
    case '[': {
      ret.type = _JSON_TOKEN_TYPE_OPEN_BRACKET; 
    } break;
    case ']': {
      ret.type = _JSON_TOKEN_TYPE_CLOSE_BRACKET; 
      ++t->at;
    } break;
    case '{': {
      ret.type = _JSON_TOKEN_TYPE_OPEN_BRACE; 
      ++t->at;
    } break;
    case '}': {
      ret.type = _JSON_TOKEN_TYPE_CLOSE_BRACE; 
      ++t->at;
    } break;
    case ',': {
      ret.type = _JSON_TOKEN_TYPE_COMMA;
      ++t->at;
    } break;
    case ':': { 
      ret.type = _JSON_TOKEN_TYPE_COLON; 
      ++t->at;
    } break;
    case 't':{
      if(t->text.count - t->at >= 4 &&
         t->text.e[t->at+1] == 'r' && 
         t->text.e[t->at+2] == 'u' && 
         t->text.e[t->at+3] == 'e')
      {
        ret.type = _JSON_TOKEN_TYPE_TRUE;
        ret.ope = t->at;
        t->at += 4;
      }
      else {
        ++t->at;
      }
    } break;
    case 'f': {
      if(t->text.count - t->at >= 5 &&
         t->text.e[t->at+1] == 'a' && 
         t->text.e[t->at+2] == 'l' && 
         t->text.e[t->at+3] == 's' &&
         t->text.e[t->at+4] == 'e')
      {
        ret.type = _JSON_TOKEN_TYPE_FALSE;
        ret.ope = t->at;
        t->at += 5;
      }
      else {
        ++t->at;
      }

    } break;
    case 'n': {
      if ((t->text.count - t->at) >= 4 && 
           t->text.e[t->at+1] == 'u' && 
           t->text.e[t->at+2] == 'l' && 
           t->text.e[t->at+3] == 'l')
      {
        ret.type = _JSON_TOKEN_TYPE_NULL;
        ret.ope = t->at;
        t->at += 4;
      }
      else {
        ++t->at;
      }

    } break;

    case '"': // strings
    {
      ++t->at;
      ret.begin = t->at;
      while(t->text.e[t->at] &&
            t->text.e[t->at] != '"') 
      {
        if(t->text.e[t->at] == '\\' && 
           t->text.e[t->at+1]) 
        {
          ++t->at;
        }
        ++t->at;
      }
      ret.type = _JSON_TOKEN_TYPE_STRING;
      ret.ope = t->at;
      ++t->at;
    } break;
    
    default: {
      // Unsigned integer
      if (is_digit(t->text.e[t->at])) {
        ret.type = _JSON_TOKEN_TYPE_UNSIGNED_INTEGER;
        while(true)
        {
          // TODO check for double dots?
          if (t->text.e[t->at] == '.') {
            ret.type = _JSON_TOKEN_TYPE_FLOATING_POINT;
          }
          else if (!is_digit(t->text.e[t->at])) {
            break;
          }
          ++t->at;
        }
        ret.ope = t->at;
      }
      // Signed integer
      else if (t->text.e[t->at] == '-' && is_digit(t->text.e[t->at+1])) {
        ++t->at; // keeps the negative sign

        // TODO: floating point
        while(is_digit(t->text.e[t->at]))
        {
          ++t->at;
        }
        ret.type = _JSON_TOKEN_TYPE_SIGNED_INTEGER;
        ret.ope = t->at;
      }      
      else {
        ++t->at;
      }
    }
  }
  return ret;
}
static void
_json_print_token(_JSON_Tokenizer* t, _JSON_Token token)  {
#if 0
  switch(token.type) {
    case _JSON_TOKEN_TYPE_OPEN_PAREN: 
    case _JSON_TOKEN_TYPE_CLOSE_PAREN:
    case _JSON_TOKEN_TYPE_SEMICOLON:
    case _JSON_TOKEN_TYPE_COLON:
    case _JSON_TOKEN_TYPE_COMMA:
    case _JSON_TOKEN_TYPE_OPEN_BRACKET:
    case _JSON_TOKEN_TYPE_CLOSE_BRACKET:
    case _JSON_TOKEN_TYPE_OPEN_BRACE:
    case _JSON_TOKEN_TYPE_CLOSE_BRACE:{
      printf("token: ");
    } break;
    case _JSON_TOKEN_TYPE_IDENTIFIER: {
      printf("identifier: ");
    } break;
    case _JSON_TOKEN_TYPE_STRING: {
      printf("string: ");
    } break;
    case _JSON_TOKEN_TYPE_EOF: {
      printf("eof");
    } break;
    case _JSON_TOKEN_TYPE_UNKNOWN: {
      printf("unknown: ");
    } break;
    
  }
#endif 
  for(U32 i = token.begin; i < token.ope; ++i) {
    printf("%c", t->text.e[i]);
  }
}

static _JSON_Node*
_json_alloc_node() {
  _JSON_Node* ret = (_JSON_Node*)malloc(sizeof(_JSON_Node)); 
  ret->left = null;
  ret->right = null;

  return ret;
}

static void
_json_set_node_key(_JSON_Node* node, _JSON_Tokenizer* j, _JSON_Token key)
{
  U32 key_len = key.ope - key.begin;
  U8* key_mem = (U8*)malloc(key_len);
  for(U32 i = 0; i < key_len; ++i) {
    key_mem[i] = j->text.e[key.begin + i];
  }
  node->key = str8(key_mem, key_len);  
}


static B32
_json_insert_node(_JSON_Node** node, _JSON_Node* new_node) {
  
  if ((*node) == null) {
    (*node) = new_node; 
    return true;
  }
  else {
    _JSON_Node* itr = (*node);
    while(itr != null) {
      SMI cmp = str8_compare_lexographically(itr->key, new_node->key);
      if (cmp > 0) {
        if (itr->left == null) {
          itr->left = new_node;
          return true;
        }
        else {
          itr = itr->left;
        }
      }
      else if (cmp < 0) {
        if (itr->right == null) {
          itr->right = new_node;
          return true;
        }
        else {
          itr = itr->right;
        }
      }
      else {
        return false;
      }
    }
    return false;
  }
}

static _JSON_Node* _json_parse_object(_JSON_Tokenizer* t);

static B32 
_json_set_value_based_on_token(_JSON_Value* value, _JSON_Tokenizer* t, _JSON_Token token) {
  B32 error = false;;
  if (token.type == _JSON_TOKEN_TYPE_UNSIGNED_INTEGER) {
    U32 number = 0;
    B32 success = str8_to_u32_range(t->text, token.begin, token.ope, &number);
    if (success) {
      value->u32 = number;
      value->type = _JSON_VALUE_TYPE_U32;
    }
    else {
      error = true;
    }
  }
  else if(token.type == _JSON_TOKEN_TYPE_SIGNED_INTEGER) {
    S32 number = 0;
    B32 success = str8_to_s32_range(t->text, token.begin, token.ope, &number);
    if (success) {
      value->s32 = number;
      value->type = _JSON_VALUE_TYPE_S32;
    }
    else {
      error = true;
    }
  }
  else if(token.type == _JSON_TOKEN_TYPE_FLOATING_POINT) {
    F32 number = 0;
    B32 success = str8_to_f32_range(t->text, token.begin, token.ope, &number);
    if (success) {
      value->f32 = number;
      value->type = _JSON_VALUE_TYPE_F32;
    }
    else {
      error = true;
    }
  }
  else if(token.type == _JSON_TOKEN_TYPE_STRING) 
  {
    U32 token_len = token.ope - token.begin;
    U8* val_mem = (U8*)malloc(token_len);
    for(U32 token_i = 0; token_i < token_len; ++token_i) {
      val_mem[token_i] = t->text.e[token.begin + token_i];
    }
    value->string = str8(val_mem, token_len);
    value->type = _JSON_VALUE_TYPE_STR8; 

  }
  else if (token.type == _JSON_TOKEN_TYPE_NULL) {
    value->type = _JSON_VALUE_TYPE_NULL;
  }
  else if (token.type == _JSON_TOKEN_TYPE_TRUE) {
    value->b32 = true;
    value->type = _JSON_VALUE_TYPE_B32;
  }
  else if (token.type == _JSON_TOKEN_TYPE_FALSE) {
    value->b32 = false;
    value->type = _JSON_VALUE_TYPE_B32;
  }
  else if (token.type == _JSON_TOKEN_TYPE_OPEN_BRACE) {
    // Parse json object
    _JSON_Node* obj = _json_parse_object(t); 
    value->obj.node = obj;
    value->type = _JSON_VALUE_TYPE_OBJECT;
  }
  return !error;
}

static _JSON_Node* 
_json_parse_object(_JSON_Tokenizer* t) {
  _JSON_Node* node = null;
  _JSON_Expect_Type expect_type = _JSON_EXPECT_TYPE_KEY_OR_CLOSE; 
  B32 is_done = false;
  B32 error = false;

  _JSON_Node* current_node = null;

  while(!is_done) {
    _JSON_Token token = _json_next_token(t);
    switch(expect_type) {
      case _JSON_EXPECT_TYPE_KEY_OR_CLOSE: {
        if (token.type == _JSON_TOKEN_TYPE_STRING) {
          current_node = _json_alloc_node();
          _json_set_node_key(current_node, t, token); 
          expect_type = _JSON_EXPECT_TYPE_COLON;
        }
        else if(token.type == _JSON_TOKEN_TYPE_CLOSE_BRACE) {
          is_done = true;
        }
        else {
          is_done = true;
          error = true;
        }
      } break;
      case _JSON_EXPECT_TYPE_COLON: {
        if ( token.type == _JSON_TOKEN_TYPE_COLON ) {
          expect_type = _JSON_EXPECT_TYPE_VALUE; 
        }
        else {
          is_done = true;
          error = true;
        }
      } break;
      case _JSON_EXPECT_TYPE_VALUE: {
        if (_json_set_value_based_on_token(&current_node->value, t, token)) {
          _json_insert_node(&node, current_node);
        }
        else {
          is_done = true;
          error = true;
        }

        expect_type = _JSON_EXPECT_TYPE_COMMA_OR_CLOSE;

    
      } break;
      case _JSON_EXPECT_TYPE_COMMA_OR_CLOSE:{
        if (token.type == _JSON_TOKEN_TYPE_COMMA) {
          expect_type = _JSON_EXPECT_TYPE_KEY_OR_CLOSE;
        }
        if (token.type == _JSON_TOKEN_TYPE_CLOSE_BRACE) {
          is_done = true; //is this right?? 
        }


      } break;
    }
  }

  return node;
}

#if JSON_DEBUG 
#include <stdio.h>
static void 
_json_print_nodes_in_order(_JSON_Node* node) {
  static U32 scope = 0;
  if (node == null) {
    return;
  }
  else {
    _json_print_nodes_in_order(node->left);
    for(U32 _i = 0; _i < scope; ++_i) 
      printf(" ");

    for (U32 i = 0; i < node->key.count; ++i)
      printf("%c", node->key.e[i]);
    printf(":");
    switch(node->value.type) {
      case _JSON_VALUE_TYPE_STR8: {
        for (U32 i = 0; i < node->value.string.count; ++i) {
          printf("%c", node->value.string.e[i]);
        }
      } break;
      case _JSON_VALUE_TYPE_U32: {
        printf("%d", node->value.u32);
      } break;
      case _JSON_VALUE_TYPE_S32: {
        printf("%d", node->value.s32);
      } break;
      case _JSON_VALUE_TYPE_F32: {
        printf("%f", node->value.f32);
      } break;
      case _JSON_VALUE_TYPE_NULL: {
        printf("null");
      } break;
      case _JSON_VALUE_TYPE_B32: {
        node->value.b32 ? printf("true") : printf("false");
      } break;
      case _JSON_VALUE_TYPE_OBJECT: {
        scope++;
        printf("{\n");
        _json_print_nodes_in_order(node->value.obj.node);

        scope--;
        for(U32 _i = 0; _i < scope; ++_i) 
          printf(" ");

        printf("}");
        break;
      }
    }
    
    printf("\n");

    _json_print_nodes_in_order(node->right);
  }
}
#endif // JSON_DEBUG

static _JSON_Tokenizer 
_json_create_tokenizer(void* png_memory, UMI png_size) {
  _JSON_Tokenizer ret = {0};
  ret.text = str8((U8*)png_memory, png_size);
  ret.at = 0;
  return ret;
}

static B32
json_read(JSON* j, void* memory, UMI size) 
{
  _JSON_Tokenizer tokenizer = _json_create_tokenizer(memory, size);
  _JSON_Token token = _json_next_token(&tokenizer);
  if (token.type != _JSON_TOKEN_TYPE_OPEN_BRACE) return false;
  _JSON_Node* node = _json_parse_object(&tokenizer);

  // print the node in order
#if JSON_DEBUG
  printf("=== Printing json tree in-order ===\n");
  _json_print_nodes_in_order(node);
#endif //JSON_DEBUG

  j->node = node;
  return true;
}


static B32 
json_read_from_blk(JSON* j, Block blk) {
  return json_read(j, blk.data, blk.size);
}

static _JSON_Node* 
_json_get(JSON* j, String8 key) {
  _JSON_Node* node = j->node;
  while(node) {
    SMI cmp = str8_compare_lexographically(key, node->key); 
    if (cmp < 0) {
      node = node->left;
    }
    else if (cmp > 0) {
      node = node->right;
    }
    else {
      return node;
    }
  }

  return node;
}

static S32* 
json_get_s32(JSON* j, String8 key) {
  _JSON_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_S32) return null;
  return &node->value.s32;
}

static U32* 
json_get_u32(JSON* j, String8 key) {
  _JSON_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_U32) return null;
  return &node->value.u32;
}

static F32* 
json_get_f32(JSON* j, String8 key) {
  _JSON_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_F32) return null;
  return &node->value.f32;
}

static B32* 
json_get_b32(JSON* j, String8 key) {
  _JSON_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_B32) return null;
  return &node->value.b32;
}

static String8* 
json_get_str8(JSON* j, String8 key) {
  _JSON_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_STR8) return null;
  return &node->value.string;
}

static JSON*
json_get_object(JSON* j, String8 key) {
  _JSON_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_OBJECT) return null;
  return &node->value.obj;
}

int main() {
 FILE* fp = fopen("test_json.json", "r");
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

  make(JSON, json);
  json_read(json, mem, len);

  JSON* one = json_get_object(json, str8_from_lit("obj"));
  U32* two = json_get_u32(one, str8_from_lit("item3"));

  printf("hello: %d", *two);

}




#ifndef MOMO_JSON
#define MOMO_JSON


typedef struct {
  struct _JSON_Object_Node* node;
} JSON_Object;


typedef struct JSON_Array {
  struct _JSON_Array_Node* head;  
  struct _JSON_Array_Node* tail;
} JSON_Array;

static B32 json_read(JSON_Object* j, void* memory, UMI size) ;
static B32 json_read_from_blk(JSON_Object* j, Block blk);
static S32* json_get_s32(JSON_Object* j, String8 key); 
static U32* json_get_u32(JSON_Object* j, String8 key); 
static F32* json_get_f32(JSON_Object* j, String8 key);
static B32* json_get_b32(JSON_Object* j, String8 key);
static String8* json_get_str8(JSON_Object* j, String8 key);
static JSON_Object* json_get_object(JSON_Object* j, String8 key);
static JSON_Array* json_get_array(JSON_Object* j, String8 key);

///////////////////////////////////
//
// IMPLEMENTATION

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
  _JSON_OBJECT_EXPECT_TYPE_OPEN,
  _JSON_OBJECT_EXPECT_TYPE_KEY_OR_CLOSE,
  _JSON_OBJECT_EXPECT_TYPE_VALUE,
  _JSON_OBJECT_EXPECT_TYPE_COMMA_OR_CLOSE,
  _JSON_OBJECT_EXPECT_TYPE_COLON,
} _JSON_Object_Expect_Type;

typedef struct {
  _JSON_Token_Type type;
  U32 begin;
  U32 ope;
}_JSON_Token;

typedef struct  {
  String8 text;
  U32 at;
} _JSON_Tokenizer;

typedef struct {
  _JSON_Value_Type type;
  union {
    B32 b32;
    U32 u32;
    S32 s32;
    String8 string;
    F32 f32;
    JSON_Object obj;
    JSON_Array arr;
  };
} _JSON_Value;

typedef struct _JSON_Object_Node {
  String8 key;
  _JSON_Value value;
  struct _JSON_Object_Node* left;
  struct _JSON_Object_Node* right;
} _JSON_Object_Node;

typedef struct _JSON_Array_Node{
  _JSON_Value value;
  struct _JSON_Array_Node* next;
} _JSON_Array_Node;


static void
_json_append_array(JSON_Array* arr, _JSON_Array_Node* node) {
  sll_append(arr->head, arr->tail, node); 
}


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
      ++t->at;
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

static _JSON_Object_Node*
_json_alloc_object_node() {
  _JSON_Object_Node* ret = (_JSON_Object_Node*)malloc(sizeof(_JSON_Object_Node)); 
  ret->left = null;
  ret->right = null;

  return ret;
}

static _JSON_Array_Node*
_json_alloc_array_node() {
  _JSON_Array_Node* ret = (_JSON_Array_Node*)malloc(sizeof(_JSON_Array_Node)); 
  ret->next = null;
  return ret;
}

static void
_json_set_node_key(_JSON_Object_Node* node, _JSON_Tokenizer* j, _JSON_Token key)
{
  U32 key_len = key.ope - key.begin;
  U8* key_mem = (U8*)malloc(key_len);
  for(U32 i = 0; i < key_len; ++i) {
    key_mem[i] = j->text.e[key.begin + i];
  }
  node->key = str8(key_mem, key_len);  
}


static B32
_json_insert_node(_JSON_Object_Node** node, _JSON_Object_Node* new_node) {
  if ((*node) == null) {
    (*node) = new_node; 
    return true;
  }
  else {
    _JSON_Object_Node* itr = (*node);
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
static B32 _json_parse_object(JSON_Object*, _JSON_Tokenizer* t);
static B32 _json_set_value_based_on_token(_JSON_Value* value, _JSON_Tokenizer* t, _JSON_Token token);

static B32
_json_parse_array(JSON_Array* arr, _JSON_Tokenizer* t) {
  B32 is_done = false;
  U32 expect_type = 0;
  B32 error = false;
  

  while(!is_done) {
    _JSON_Token token = _json_next_token(t);

    if (expect_type == 0) {
      _JSON_Value v = {0};
      if(_json_set_value_based_on_token(&v, t, token)) {
        _JSON_Array_Node* array_node = _json_alloc_array_node(); 
        array_node->value = v;
        _json_append_array(arr, array_node);
      }
      else {
        is_done = true;
        error = true;
      }
      expect_type = 1;
    }
    else if (expect_type == 1) {
      if (token.type == _JSON_TOKEN_TYPE_COMMA) {
        expect_type = 0;
      }
      else if (token.type == _JSON_TOKEN_TYPE_CLOSE_BRACKET) {
        is_done = true;
      }
      else {
        is_done = true;
        error = true;
      }
    }
  }
 
  return !error;

}

static B32 
_json_set_value_based_on_token(_JSON_Value* value, _JSON_Tokenizer* t, _JSON_Token token) {
  B32 error = false;
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
    if (_json_parse_object(&value->obj, t)) {
      value->type = _JSON_VALUE_TYPE_OBJECT;
    }
    else {
      error = true; 
    }
  }
  else if (token.type == _JSON_TOKEN_TYPE_OPEN_BRACKET) {
    if (_json_parse_array(&value->arr, t)) {
      value->type = _JSON_VALUE_TYPE_ARRAY;
    }
    else {
      error = true;
    }
  }
  return !error;
}

static B32  
_json_parse_object(JSON_Object* obj, _JSON_Tokenizer* t) {
  _JSON_Object_Node* node = null;
  _JSON_Object_Expect_Type expect_type = _JSON_OBJECT_EXPECT_TYPE_KEY_OR_CLOSE; 
  B32 is_done = false;
  B32 error = false;

  _JSON_Object_Node* current_node = null;

  while(!is_done) {
    _JSON_Token token = _json_next_token(t);
    switch(expect_type) {
      case _JSON_OBJECT_EXPECT_TYPE_KEY_OR_CLOSE: {
        if (token.type == _JSON_TOKEN_TYPE_STRING) {
          current_node = _json_alloc_object_node();
          _json_set_node_key(current_node, t, token); 
          expect_type = _JSON_OBJECT_EXPECT_TYPE_COLON;
        }
        else if(token.type == _JSON_TOKEN_TYPE_CLOSE_BRACE) {
          is_done = true;
        }
        else {
          is_done = true;
          error = true;
        }
      } break;
      case _JSON_OBJECT_EXPECT_TYPE_COLON: {
        if ( token.type == _JSON_TOKEN_TYPE_COLON ) {
          expect_type = _JSON_OBJECT_EXPECT_TYPE_VALUE; 
        }
        else {
          is_done = true;
          error = true;
        }
      } break;
      case _JSON_OBJECT_EXPECT_TYPE_VALUE: {
        if (_json_set_value_based_on_token(&current_node->value, t, token)) {
          _json_insert_node(&node, current_node);
        }
        else {
          is_done = true;
          error = true;
        }

        expect_type = _JSON_OBJECT_EXPECT_TYPE_COMMA_OR_CLOSE;

    
      } break;
      case _JSON_OBJECT_EXPECT_TYPE_COMMA_OR_CLOSE:{
        if (token.type == _JSON_TOKEN_TYPE_COMMA) {
          expect_type = _JSON_OBJECT_EXPECT_TYPE_KEY_OR_CLOSE;
        }
        if (token.type == _JSON_TOKEN_TYPE_CLOSE_BRACE) {
          is_done = true; 
        }
      } break;
    }
  }
  
  obj->node = node; 
  return !error;
}
#if JSON_DEBUG 
#include <stdio.h>

static U32 scope = 0;
static void _json_print_nodes_in_order(_JSON_Object_Node* node);

static void
_json_print_value(_JSON_Value* value) {
  switch(value->type) {
    case _JSON_VALUE_TYPE_STR8: {
      printf("\"");
      for (U32 i = 0; i < value->string.count; ++i) {
        printf("%c", value->string.e[i]);
      }
      printf("\"");
    } break;
    case _JSON_VALUE_TYPE_U32: {
      printf("%d", value->u32);
    } break;
    case _JSON_VALUE_TYPE_S32: {
      printf("%d", value->s32);
    } break;
    case _JSON_VALUE_TYPE_F32: {
      printf("%f", value->f32);
    } break;
    case _JSON_VALUE_TYPE_NULL: {
      printf("null");
    } break;
    case _JSON_VALUE_TYPE_B32: {
      value->b32 ? printf("true") : printf("false");
    } break;
    case _JSON_VALUE_TYPE_OBJECT: {
      scope++;
      printf("{\n");
      _json_print_nodes_in_order(value->obj.node);
      scope--;
      for(U32 _i = 0; _i < scope; ++_i) 
        printf(" ");
      printf("}");
    } break;
    case _JSON_VALUE_TYPE_ARRAY: {
      printf("[");
      for (_JSON_Array_Node* itr = value->arr.head;
          itr != null;
          itr = itr->next) 
      {
        _json_print_value(&itr->value);
        if (itr->next != null)
          printf(",");
      }
      printf("]");
           

    } break;
  }
}

static void 
_json_print_nodes_in_order(_JSON_Object_Node* node) {
  if (node == null) {
    return;
  }
  else {
    _json_print_nodes_in_order(node->left);

    for(U32 i = 0; i < scope; ++i) 
      printf(" ");
    for (U32 i = 0; i < node->key.count; ++i)
      printf("%c", node->key.e[i]);
    printf(":");

    _json_print_value(&node->value);
    
    printf("\n");

    _json_print_nodes_in_order(node->right);
  }
}
static _JSON_Tokenizer 
_json_create_tokenizer(void* png_memory, UMI png_size) {
  _JSON_Tokenizer ret = {0};
  ret.text = str8((U8*)png_memory, png_size);
  ret.at = 0;
  return ret;
}
static _JSON_Object_Node* 
_json_get(JSON_Object* j, String8 key) {
  _JSON_Object_Node* node = j->node;
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
static B32
json_read(JSON_Object* j, void* memory, UMI size) 
{
  _JSON_Tokenizer tokenizer = _json_create_tokenizer(memory, size);
  _JSON_Token token = _json_next_token(&tokenizer);
  if (token.type != _JSON_TOKEN_TYPE_OPEN_BRACE) return false;
  _json_parse_object(j, &tokenizer);

  // print the node in order
#if JSON_DEBUG
  printf("=== Printing json tree in-order ===\n");
  _json_print_nodes_in_order(j->node);
#endif //JSON_DEBUG
  return true;
}


static B32 
json_read_from_blk(JSON_Object* j, Block blk) {
  return json_read(j, blk.data, blk.size);
}

static S32* 
json_get_s32(JSON_Object* j, String8 key) {
  _JSON_Object_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_S32) return null;
  return &node->value.s32;
}

static U32* 
json_get_u32(JSON_Object* j, String8 key) {
  _JSON_Object_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_U32) return null;
  return &node->value.u32;
}

static F32* 
json_get_f32(JSON_Object* j, String8 key) {
  _JSON_Object_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_F32) return null;
  return &node->value.f32;
}

static B32* 
json_get_b32(JSON_Object* j, String8 key) {
  _JSON_Object_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_B32) return null;
  return &node->value.b32;
}

static String8* 
json_get_str8(JSON_Object* j, String8 key) {
  _JSON_Object_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_STR8) return null;
  return &node->value.string;
}

static JSON_Object*
json_get_object(JSON_Object* j, String8 key) {
  _JSON_Object_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_OBJECT) return null;
  return &node->value.obj;
}

static JSON_Array*
json_get_array(JSON_Object* j, String8 key) {
  _JSON_Object_Node* node = _json_get(j, key);
  if (!node) return null;
  if (node->value.type != _JSON_VALUE_TYPE_ARRAY) return null;
  return &node->value.arr;
}
#endif // JSON_DEBUG



#endif //MOMO_JSON

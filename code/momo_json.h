#ifndef MOMO_JSON
#define MOMO_JSON

#define KEY_V2 1

struct json_object_t {
  struct _json_object_node_t* node;
};

struct json_t {
  // for tokenizing
  str8_t text;
  u32_t at;



  // for use
  json_object_t root;
};


struct json_array_t{
  struct _json_array_node_t* head;  
  struct _json_array_node_t* tail;
};

static b32_t json_read(json_t* j, void* json_string, umi_t json_string_size, arena_t* arena);

#if 0
static b32_t json_read(json_object_t* j, void* json_string, umi_t json_string_size, arena_t* arena);
static s32_t* json_get_s32(json_object_t* j, str8_t key); 
static u32_t* json_get_u32(json_object_t* j, str8_t key); 
static f32_t* json_get_f32(json_object_t* j, str8_t key);
static b32_t* json_get_b32(json_object_t* j, str8_t key);
static str8_t* json_get_str8(json_object_t* j, str8_t key);
static json_object_t* json_get_object(json_object_t* j, str8_t key);
static json_array_t* json_get_array(json_object_t* j, str8_t key);
#endif

//
// IMPLEMENTATION
//

enum _json_token_type_t {
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
};

enum  _json_value_type_t {
  _JSON_VALUE_TYPE_U32,
  _JSON_VALUE_TYPE_S32,
  _JSON_VALUE_TYPE_F32,
  _JSON_VALUE_TYPE_B32,
  _JSON_VALUE_TYPE_STR8,
  _JSON_VALUE_TYPE_ARRAY,
  _JSON_VALUE_TYPE_OBJECT,
  _JSON_VALUE_TYPE_NULL
};

enum  _json_object_expect_type_t {
  _JSON_OBJECT_EXPECT_TYPE_OPEN,
  _JSON_OBJECT_EXPECT_TYPE_KEY_OR_CLOSE,
  _JSON_OBJECT_EXPECT_TYPE_VALUE,
  _JSON_OBJECT_EXPECT_TYPE_COMMA_OR_CLOSE,
  _JSON_OBJECT_EXPECT_TYPE_COLON,
};

struct _json_token_t{
  _json_token_type_t type;
  u32_t begin;
  u32_t ope;
};


struct _json_value_t {
  _json_value_type_t type;
  union {
    b32_t b32;
    u32_t u32_t;
    s32_t s32_t;
    str8_t string;
    f32_t f32_t;
    json_object_t obj;
    json_array_t arr;
  };
};

#if KEY_V2
struct json_key_t {
  u32_t begin;
  u32_t ope;
};
#endif

struct _json_object_node_t {
#if KEY_V2
  json_key_t key;
#else
  str8_t key;
#endif
  _json_value_t value;
  struct _json_object_node_t* left;
  struct _json_object_node_t* right;
};

struct _json_array_node_t{
  _json_value_t value;
  struct _json_array_node_t* next;
};


static void
_json_append_array(json_array_t* arr, _json_array_node_t* node) {
  sll_append(arr->head, arr->tail, node); 
}


static void
_json_eat_ignorables(json_t* t) {
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
static _json_token_t
_json_next_token(json_t* t) {
  _json_eat_ignorables(t);
  
  _json_token_t ret = {0};
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
_json_set_node_key(_json_object_node_t* node, json_t* j, _json_token_t key, arena_t* ba)
{

#if KEY_V2
  node->key.begin = key.begin;
  node->key.ope = key.ope;
#else
  u32_t key_len = key.ope - key.begin;
  u8_t* key_mem = arena_push_arr(u8_t, ba, key_len);
  for(u32_t i = 0; i < key_len; ++i) {
    key_mem[i] = j->text.e[key.begin + i];
  }
  node->key = str8(key_mem, key_len);  
#endif
}

static s32_t 
_json_compare_keys(json_t* t, json_key_t lhs, json_key_t rhs) 
{
  for (u32_t i = lhs.begin; i < lhs.ope && i < rhs.ope; ++i) {
    if (t->text.e[i] == t->text.e[i]) 
      continue;
    else {
      return t->text.e[i] - t->text.e[i];
    }
  }

  // Edge case for strings like:
  // lhs == "asd" and rhs == "asdfg"
  u32_t lhs_count = lhs.ope - lhs.begin;
  u32_t rhs_count = rhs.ope - rhs.begin;

  if (lhs_count == rhs_count) 
  {
    return 0;
  }
  else 
  {
    return lhs_count - rhs_count;
  }
 
}

static b32_t
_json_insert_node(json_t* t, _json_object_node_t** node, _json_object_node_t* new_node) {
  if ((*node) == nullptr) {
    (*node) = new_node; 
    return true;
  }
  else {
    _json_object_node_t* itr = (*node);
    while(itr != nullptr) {
      s32_t cmp = _json_compare_keys(t, itr->key, new_node->key);
      if (cmp > 0) {
        if (itr->left == nullptr) {
          itr->left = new_node;
          return true;
        }
        else {
          itr = itr->left;
        }
      }
      else if (cmp < 0) {
        if (itr->right == nullptr) {
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
static b32_t 
_json_parse_object(json_object_t*, json_t* t, arena_t* ba);

static b32_t _json_set_value_based_on_token(_json_value_t* value, json_t* t, _json_token_t token, arena_t* ba);

static b32_t
_json_parse_array(json_array_t* arr, json_t* t, arena_t* ba) {
  b32_t is_done = false;
  u32_t expect_type = 0;
  b32_t error = false;
  

  while(!is_done) {
    _json_token_t token = _json_next_token(t);

    if (expect_type == 0) {
      _json_value_t v = {0};
      if(_json_set_value_based_on_token(&v, t, token, ba)) {
        _json_array_node_t* array_node = arena_push(_json_array_node_t, ba); 
        if (array_node) {
          array_node->value = v;
          _json_append_array(arr, array_node);

        }
        else {
          is_done = true;
          error = true;
        }
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

static b32_t 
_json_set_value_based_on_token(_json_value_t* value, 
                               json_t* t, 
                               _json_token_t token,
                               arena_t* ba) 
{
  b32_t error = false;
  if (token.type == _JSON_TOKEN_TYPE_UNSIGNED_INTEGER) {
    u32_t number = 0;
    b32_t success = str8_to_u32_range(t->text, token.begin, token.ope, &number);
    if (success) {
      value->u32_t = number;
      value->type = _JSON_VALUE_TYPE_U32;
    }
    else {
      error = true;
    }
  }
  else if(token.type == _JSON_TOKEN_TYPE_SIGNED_INTEGER) {
    s32_t number = 0;
    b32_t success = str8_to_s32_range(t->text, token.begin, token.ope, &number);
    if (success) {
      value->s32_t = number;
      value->type = _JSON_VALUE_TYPE_S32;
    }
    else {
      error = true;
    }
  }
  else if(token.type == _JSON_TOKEN_TYPE_FLOATING_POINT) {
    f32_t number = 0;
    b32_t success = str8_to_f32_range(t->text, token.begin, token.ope, &number);
    if (success) {
      value->f32_t = number;
      value->type = _JSON_VALUE_TYPE_F32;
    }
    else {
      error = true;
    }
  }
  else if(token.type == _JSON_TOKEN_TYPE_STRING) 
  {
    u32_t token_len = token.ope - token.begin;
    u8_t* val_mem = arena_push_arr(u8_t, ba, token_len);     
    if (val_mem) {
      for(u32_t token_i = 0; token_i < token_len; ++token_i) {
        val_mem[token_i] = t->text.e[token.begin + token_i];
      }
      value->string = str8(val_mem, token_len);
      value->type = _JSON_VALUE_TYPE_STR8; 
    }
    else {
      error = true;
    }
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
    if (_json_parse_object(&value->obj, t, ba)) {
      value->type = _JSON_VALUE_TYPE_OBJECT;
    }
    else {
      error = true; 
    }
  }
  else if (token.type == _JSON_TOKEN_TYPE_OPEN_BRACKET) {
    if (_json_parse_array(&value->arr, t, ba)) {
      value->type = _JSON_VALUE_TYPE_ARRAY;
    }
    else {
      error = true;
    }
  }
  return !error;
}

static b32_t  
_json_parse_object(json_object_t* obj, json_t* t, arena_t* ba) {
  _json_object_node_t* node = nullptr;
  _json_object_expect_type_t expect_type = _JSON_OBJECT_EXPECT_TYPE_KEY_OR_CLOSE; 
  b32_t is_done = false;
  b32_t error = false;

  _json_object_node_t* current_node = nullptr;

  while(!is_done) {
    _json_token_t token = _json_next_token(t);
    switch(expect_type) {
      case _JSON_OBJECT_EXPECT_TYPE_KEY_OR_CLOSE: {
        if (token.type == _JSON_TOKEN_TYPE_STRING) {
          current_node = arena_push(_json_object_node_t, ba);
          if (!current_node) {
            is_done = true;
            error = true;
          }
          else {
            _json_set_node_key(current_node, t, token, ba); 
            expect_type = _JSON_OBJECT_EXPECT_TYPE_COLON;
          }
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
        if (_json_set_value_based_on_token(&current_node->value, t, token, ba)) {
          _json_insert_node(t, &node, current_node);
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

static u32_t scope = 0;
static void _json_print_nodes_in_order(json_t* t,  _json_object_node_t* node);

static void
_json_print_token(json_t* t, _json_token_t token)  {
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
  for(u32_t i = token.begin; i < token.ope; ++i) {
    printf("%c", t->text.e[i]);
  }
}

static void
_json_print_value(json_t* t, _json_value_t* value) {
  switch(value->type) {
    case _JSON_VALUE_TYPE_STR8: {
      printf("\"");
      for (u32_t i = 0; i < value->string.count; ++i) {
        printf("%c", value->string.e[i]);
      }
      printf("\"");
    } break;
    case _JSON_VALUE_TYPE_U32: {
      printf("%d", value->u32_t);
    } break;
    case _JSON_VALUE_TYPE_S32: {
      printf("%d", value->s32_t);
    } break;
    case _JSON_VALUE_TYPE_F32: {
      printf("%f", value->f32_t);
    } break;
    case _JSON_VALUE_TYPE_NULL: {
      printf("nullptr");
    } break;
    case _JSON_VALUE_TYPE_B32: {
      value->b32 ? printf("true") : printf("false");
    } break;
    case _JSON_VALUE_TYPE_OBJECT: {
      scope++;
      printf("{\n");
      _json_print_nodes_in_order(t, value->obj.node);
      scope--;
      for(u32_t _i = 0; _i < scope; ++_i) 
        printf(" ");
      printf("}");
    } break;
    case _JSON_VALUE_TYPE_ARRAY: {
      printf("[");
      for (_json_array_node_t* itr = value->arr.head;
          itr != nullptr;
          itr = itr->next) 
      {
        _json_print_value(t, &itr->value);
        if (itr->next != nullptr)
          printf(",");
      }
      printf("]");
           

    } break;
  }
}

static void 
_json_print_nodes_in_order(json_t* t, _json_object_node_t* node) 
{
  if (node == nullptr) 
  {
    return;
  }
  else 
  {
    _json_print_nodes_in_order(t, node->left);

    for(u32_t i = 0; i < scope; ++i) 
      printf(" ");
    for (u32_t i = node->key.begin; i < node->key.ope; ++i)
      printf("%c", t->text.e[i]);
    printf(":");

    _json_print_value(t, &node->value);
    
    printf("\n");

    _json_print_nodes_in_order(t, node->right);
  }
}
static json_t 
_json_create_tokenizer(u8_t* json_string, umi_t json_string_size) {
  json_t ret = {0};
  ret.text = str8(json_string, json_string_size);
  ret.at = 0;
  return ret;
}

#if 0
static _json_object_node_t* 
_json_get(json_object_t* j, str8_t key) {
  _json_object_node_t* node = j->node;
  while(node) {
    smi_t cmp = str8_compare_lexographically(key, node->key); 
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
#endif

#if 0
static b32_t
json_read(json_object_t* j, u8_t* json_string, umi_t json_string_size, arena_t* ba) 
{
  json_t tokenizer = _json_create_tokenizer(json_string, json_string_size);
  _json_token_t token = _json_next_token(&tokenizer);
  if (token.type != _JSON_TOKEN_TYPE_OPEN_BRACE) return false;
  if (!_json_parse_object(j, &tokenizer, ba)) return false;

  // print the node in order
#if JSON_DEBUG
  printf("=== Printing json tree in-order ===\n");
  _json_print_nodes_in_order(&tokenizer, j->node);
#endif //JSON_DEBUG
  return true;
}
#endif
static b32_t
json_read(json_t* j, u8_t* json_string, umi_t json_string_size, arena_t* ba) 
{
  j->text = str8(json_string, json_string_size);
  j->at = 0;
  _json_token_t token = _json_next_token(j);
  if (token.type != _JSON_TOKEN_TYPE_OPEN_BRACE) return false;
  if (!_json_parse_object(&j->root, j, ba)) return false;

  // print the node in order
#if JSON_DEBUG
  printf("=== Printing json tree in-order ===\n");
  _json_print_nodes_in_order(j, j->root.node);
#endif //JSON_DEBUG
  return true;
}
#if 0
static s32_t* 
json_get_s32(json_object_t* j, str8_t key) {
  _json_object_node_t* node = _json_get(j, key);
  if (!node) return nullptr;
  if (node->value.type != _JSON_VALUE_TYPE_S32) return nullptr;
  return &node->value.s32_t;
}

static u32_t* 
json_get_u32(json_object_t* j, str8_t key) {
  _json_object_node_t* node = _json_get(j, key);
  if (!node) return nullptr;
  if (node->value.type != _JSON_VALUE_TYPE_U32) return nullptr;
  return &node->value.u32_t;
}

static f32_t* 
json_get_f32(json_object_t* j, str8_t key) {
  _json_object_node_t* node = _json_get(j, key);
  if (!node) return nullptr;
  if (node->value.type != _JSON_VALUE_TYPE_F32) return nullptr;
  return &node->value.f32_t;
}

static b32_t* 
json_get_b32(json_object_t* j, str8_t key) {
  _json_object_node_t* node = _json_get(j, key);
  if (!node) return nullptr;
  if (node->value.type != _JSON_VALUE_TYPE_B32) return nullptr;
  return &node->value.b32;
}

static str8_t* 
json_get_str8(json_object_t* j, str8_t key) {
  _json_object_node_t* node = _json_get(j, key);
  if (!node) return nullptr;
  if (node->value.type != _JSON_VALUE_TYPE_STR8) return nullptr;
  return &node->value.string;
}

static json_object_t*
json_get_object(json_object_t* j, str8_t key) {
  _json_object_node_t* node = _json_get(j, key);
  if (!node) return nullptr;
  if (node->value.type != _JSON_VALUE_TYPE_OBJECT) return nullptr;
  return &node->value.obj;
}

static json_array_t*
json_get_array(json_object_t* j, str8_t key) {
  _json_object_node_t* node = _json_get(j, key);
  if (!node) return nullptr;
  if (node->value.type != _JSON_VALUE_TYPE_ARRAY) return nullptr;
  return &node->value.arr;
}
#endif
#endif // JSON_DEBUG



#endif //MOMO_JSON

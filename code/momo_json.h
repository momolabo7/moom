#ifndef MOMO_JSON
#define MOMO_JSON

// The "key" of a JSON entry, which can only be a string.
struct json_key_t {
  u32_t begin;
  u32_t ope;
};


// Represents a JSON array, which is a linked list
// of nodes containing more values.
struct json_array_node_t;
struct json_array_t {
  json_array_node_t* head;
  json_array_node_t* tail;
};

// Represents a JSON element, which is a string.
struct json_element_t {
  u32_t begin;
  u32_t ope;
};

enum json_value_type_t {
  JSON_VALUE_TYPE_ELEMENT, 
  JSON_VALUE_TYPE_ARRAY,
  JSON_VALUE_TYPE_OBJECT,
};

// An object contains a bunch of entries
struct json_object_t {
  struct _json_entry_t* head; // points to the first entry
};

struct json_value_t {
  json_value_type_t type;
  union {
    json_element_t element;
    json_array_t array;
    json_object_t object;
  };
};

// Every json 'entry' is a key/value pair.
struct _json_entry_t {
  json_key_t key;
  json_value_t value;

  // For simplified BST data structure.
  _json_entry_t* left;
  _json_entry_t* right;
};

struct json_array_node_t {
  json_array_node_t* next;
  json_value_t value;
};

struct json_t {
  // for tokenizing
  str8_t text;
  u32_t at;

  // The 'root' item in a JSON file is an object type.
  json_object_t root;
};

static b32_t json_read(json_t* j, u8_t* json_string, u32_t json_string_size, arena_t* arena);

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

enum  _json_expect_type_t {
  _JSON_EXPECT_TYPE_OPEN,
  _JSON_EXPECT_TYPE_KEY_OR_CLOSE,
  _JSON_EXPECT_TYPE_VALUE,
  _JSON_EXPECT_TYPE_COMMA_OR_CLOSE,
  _JSON_EXPECT_TYPE_COLON,
};

struct _json_token_t {
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




static void
_json_append_array(json_array_t* arr, json_array_node_t* node) {
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
        t->at += 4;
        ret.ope = t->at;
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
        t->at += 5;
        ret.ope = t->at;
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
        t->at += 4;
        ret.ope = t->at;
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



static s32_t 
_json_compare_keys(json_t* t, json_key_t lhs, json_key_t rhs) 
{
  for (u32_t i = lhs.begin, j = rhs.begin;
       i < lhs.ope && j < rhs.ope; ++i, ++j) 
  {
    if (t->text.e[i] == t->text.e[j]) 
      continue;
    else {
      return t->text.e[i] - t->text.e[j];
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
_json_insert_entry(json_t* t, _json_entry_t** entry, _json_entry_t* new_entry) {
  if ((*entry) == nullptr) {
    (*entry) = new_entry; 
    return true;
  }
  else {
    _json_entry_t* itr = (*entry);
    while(itr != nullptr) {
      s32_t cmp = _json_compare_keys(t, itr->key, new_entry->key);
      if (cmp > 0) {
        if (itr->left == nullptr) {
          itr->left = new_entry;
          return true;
        }
        else {
          itr = itr->left;
        }
      }
      else if (cmp < 0) {
        if (itr->right == nullptr) {
          itr->right = new_entry;
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

static b32_t _json_set_value_based_on_token(json_t* t, json_value_t* value,  _json_token_t token, arena_t* ba);

static b32_t
_json_parse_array(json_array_t* arr, json_t* t, arena_t* ba) {
  b32_t is_done = false;
  u32_t expect_type = 0;
  b32_t error = false;
  

  while(!is_done) {
    _json_token_t token = _json_next_token(t);

    if (expect_type == 0) {
      json_value_t v = {0};
      if(_json_set_value_based_on_token(t, &v, token, ba)) {
        json_array_node_t* array_node = arena_push(json_array_node_t, ba); 
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
_json_set_value_based_on_token(json_t* t, json_value_t* value, _json_token_t token, arena_t* ba) 
{
  b32_t error = false;
  if (token.type == _JSON_TOKEN_TYPE_UNSIGNED_INTEGER) {
    value->type = JSON_VALUE_TYPE_ELEMENT;
    value->element.begin = token.begin;
    value->element.ope = token.ope;
  }
  else if(token.type == _JSON_TOKEN_TYPE_SIGNED_INTEGER) {
    value->type = JSON_VALUE_TYPE_ELEMENT;
    value->element.begin = token.begin;
    value->element.ope = token.ope;
  }
  else if(token.type == _JSON_TOKEN_TYPE_FLOATING_POINT) {
    value->type = JSON_VALUE_TYPE_ELEMENT;
    value->element.begin = token.begin;
    value->element.ope = token.ope;
  }
  else if(token.type == _JSON_TOKEN_TYPE_STRING) 
  {
    value->type = JSON_VALUE_TYPE_ELEMENT;
    value->element.begin = token.begin;
    value->element.ope = token.ope;
  }
  else if (token.type == _JSON_TOKEN_TYPE_NULL) {
    value->type = JSON_VALUE_TYPE_ELEMENT;
    value->element.begin = token.begin;
    value->element.ope = token.ope;
  }
  else if (token.type == _JSON_TOKEN_TYPE_TRUE) {
    value->type = JSON_VALUE_TYPE_ELEMENT;
    value->element.begin = token.begin;
    value->element.ope = token.ope;
  }
  else if (token.type == _JSON_TOKEN_TYPE_FALSE) {
    value->type = JSON_VALUE_TYPE_ELEMENT;
    value->element.begin = token.begin;
    value->element.ope = token.ope;
  }
  else if (token.type == _JSON_TOKEN_TYPE_OPEN_BRACE) {
    // Parse json object
    if (_json_parse_object(&value->object, t, ba)) {
      value->type = JSON_VALUE_TYPE_OBJECT;
    }
    else {
      error = true; 
    }
  }
  else if (token.type == _JSON_TOKEN_TYPE_OPEN_BRACKET) {
    if (_json_parse_array(&value->array, t, ba)) {
      value->type = JSON_VALUE_TYPE_ARRAY;
    }
    else {
      error = true;
    }
  }
  return !error;
}

static b32_t  
_json_parse_object(json_object_t* obj, json_t* t, arena_t* ba) {
  _json_entry_t* entry = nullptr;
  _json_expect_type_t expect_type = _JSON_EXPECT_TYPE_KEY_OR_CLOSE; 
  b32_t is_done = false;
  b32_t error = false;

  _json_entry_t* current_entry = nullptr;

  while(!is_done) {
    _json_token_t token = _json_next_token(t);
    switch(expect_type) {
      case _JSON_EXPECT_TYPE_KEY_OR_CLOSE: {
        if (token.type == _JSON_TOKEN_TYPE_STRING) {
          current_entry = arena_push(_json_entry_t, ba);
          if (!current_entry) {
            is_done = true;
            error = true;
          }
          else {
            current_entry->key.begin = token.begin;
            current_entry->key.ope = token.ope;
            expect_type = _JSON_EXPECT_TYPE_COLON;
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
        if (_json_set_value_based_on_token(t, &current_entry->value, token, ba)) {
          _json_insert_entry(t, &entry, current_entry);
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
          is_done = true; 
        }
      } break;
    }
  }
  
  obj->head = entry; 
  return !error;
}
#if JSON_DEBUG 
#include <stdio.h>

static u32_t scope = 0;
static void _json_print_entries_in_order(json_t* t,  _json_entry_t* entry);

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
_json_print_value(json_t* t, json_value_t* value) {
  switch(value->type) {
    case JSON_VALUE_TYPE_ELEMENT: 
    {
      for(u32_t _i = value->element.begin;
          _i < value->element.ope;
          ++_i)
      {
        printf("%c", t->text.e[_i]);
      }
    } break;

    case JSON_VALUE_TYPE_OBJECT: 
    {
      scope++;
      printf("{\n");
      _json_print_entries_in_order(t, value->object.head);
      scope--;
      for(u32_t _i = 0; _i < scope; ++_i) 
        printf(" ");
      printf("}");
    } break;
    case JSON_VALUE_TYPE_ARRAY: 
    {
      printf("[");
      for (json_array_node_t* itr = value->array.head;
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
_json_print_entries_in_order(json_t* t, _json_entry_t* entry) 
{
  if (entry == nullptr) 
  {
    return;
  }
  else 
  {
    _json_print_entries_in_order(t, entry->left);

    for(u32_t i = 0; i < scope; ++i) 
    {
      printf(" ");
    }

    for (u32_t i = entry->key.begin; i < entry->key.ope; ++i) 
    {
      printf("%c", t->text.e[i]);
    }
    printf(":");

    _json_print_value(t, &entry->value);
    
    printf("\n");

    _json_print_entries_in_order(t, entry->right);
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

static b32_t
json_read(json_t* j, u8_t* json_string, u32_t json_string_size, arena_t* ba) 
{
  j->text = str8(json_string, json_string_size);
  j->at = 0;
  _json_token_t token = _json_next_token(j);
  if (token.type != _JSON_TOKEN_TYPE_OPEN_BRACE) return false;
  if (!_json_parse_object(&j->root, j, ba)) return false;

  // print the node in order
#if JSON_DEBUG
  printf("=== Printing json tree in-order ===\n");
  _json_print_entries_in_order(j, j->root.head);
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

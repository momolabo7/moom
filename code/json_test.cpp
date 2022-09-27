#include <stdio.h>
#include <stdlib.h>

#include "momo.h"

typedef enum  {
  JSON_TOKEN_TYPE_UNKNOWN,
  JSON_TOKEN_TYPE_COLON,
  JSON_TOKEN_TYPE_OPEN_BRACKET,
  JSON_TOKEN_TYPE_CLOSE_BRACKET,
  JSON_TOKEN_TYPE_OPEN_BRACE,
  JSON_TOKEN_TYPE_CLOSE_BRACE,
  JSON_TOKEN_TYPE_COMMA,
  JSON_TOKEN_TYPE_NULL,
  
  JSON_TOKEN_TYPE_IDENTIFIER,
  JSON_TOKEN_TYPE_STRING,
  JSON_TOKEN_TYPE_UNSIGNED_INTEGER,
  JSON_TOKEN_TYPE_SIGNED_INTEGER,
  
  JSON_TOKEN_TYPE_EOF
} Json_Token_Type;


typedef enum {
  JSON_EXPECT_TYPE_OPEN,
  JSON_EXPECT_TYPE_KEY_OR_CLOSE,
  JSON_EXPECT_TYPE_VALUE,
  JSON_EXPECT_TYPE_COMMA_OR_CLOSE,
  JSON_EXPECT_TYPE_COLON,
} Json_Expect_Type;

typedef struct {
  Json_Token_Type type;
  
  U32 begin;
  U32 ope;
}Json_Token;

typedef struct  {
    String8 text;
    U32 at;
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
  ret.text = str8((U8*)mem, len);


  return ret;
}



static void
json_eat_ignorables(Json_Tokenizer* t) {
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

static Json_Token
json_next_token(Json_Tokenizer* t) {
  json_eat_ignorables(t);
  
  Json_Token ret = {};
  ret.begin = t->at;
  ret.ope = t->at + 1;
  
  
  switch(t->text.e[t->at]) {
    case '\0': {
      ret.type = JSON_TOKEN_TYPE_EOF; 
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
      ret.type = JSON_TOKEN_TYPE_STRING;
      ret.ope = t->at;
      ++t->at;
    } break;
    
    default: {
      // Unsigned integer
      if (is_digit(t->text.e[t->at])) {
        while(is_digit(t->text.e[t->at]))
        {
          ++t->at;
        }
        ret.type = JSON_TOKEN_TYPE_UNSIGNED_INTEGER;
        ret.ope = t->at;
      }
      else if (t->text.e[t->at] == '-' && is_digit(t->text.e[t->at+1])) {
        ++t->at;
        while(is_digit(t->text.e[t->at]))
        {
          ++t->at;
        }
        ret.type = JSON_TOKEN_TYPE_SIGNED_INTEGER;
        ret.ope = t->at;
      }
      else if ((t->text.count - t->at) >= 4 && 
               t->text.e[t->at] == 'n' && 
               t->text.e[t->at+1] == 'u' && 
               t->text.e[t->at+2] == 'l' && 
               t->text.e[t->at+3] == 'l')
      {
        ret.type = JSON_TOKEN_TYPE_NULL;
        t->at += 4;
      }

      else {
        ret.type = JSON_TOKEN_TYPE_UNKNOWN;
      }
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
    printf("%c", t->text.e[i]);
  }
}

typedef enum  {
  JSON_VALUE_TYPE_U32,
  JSON_VALUE_TYPE_S32,
  JSON_VALUE_TYPE_F32,
  JSON_VALUE_TYPE_B32,
  JSON_VALUE_TYPE_STR8,
  JSON_VALUE_TYPE_ARRAY,
  JSON_VALUE_TYPE_OBJECT,
  JSON_VALUE_TYPE_NULL
} Json_Value_Type;

typedef struct Json_Node{
  String8 key;

  Json_Value_Type value_type;
  union {
    B32 value_b32;
    U32 value_u32;
    S32 value_s32;
    String8 value_str8;
    F32 value_f32;
    Json_Node* value_obj;
  };

  struct Json_Node* left;
  struct Json_Node* right;
    
} Json_Node;

static Json_Node*
json_alloc_node() {
  Json_Node* ret = (Json_Node*)malloc(sizeof(Json_Node)); 
  ret->left = null;
  ret->right = null;

  return ret;
}

static void
json_set_node_key(Json_Node* node, Json_Tokenizer* j, Json_Token key)
{
  U32 key_len = key.ope - key.begin;
  U8* key_mem = (U8*)malloc(key_len);
  for(U32 i = 0; i < key_len; ++i) {
    key_mem[i] = j->text.e[key.begin + i];
  }
  node->key = str8(key_mem, key_len);  
}


static B32
json_insert_node(Json_Node** root, Json_Node* new_node) {
  
  if ((*root) == null) {
    (*root) = new_node; 
    return true;
  }
  else {
    Json_Node* itr = (*root);
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

static Json_Node* 
json_parse_object(Json_Tokenizer* j) {
  Json_Node* root = null;
  Json_Expect_Type expect_type = JSON_EXPECT_TYPE_KEY_OR_CLOSE; 
  B32 is_done = false;
  B32 error = false;

  Json_Node* current_node = null;

  while(!is_done) {
    Json_Token token = json_next_token(j);
    switch(expect_type) {
      case JSON_EXPECT_TYPE_KEY_OR_CLOSE: {
        if (token.type == JSON_TOKEN_TYPE_STRING) {
          current_node = json_alloc_node();
          json_set_node_key(current_node, j, token); 
          
          expect_type = JSON_EXPECT_TYPE_COLON;
        }
        else if(token.type == JSON_TOKEN_TYPE_CLOSE_BRACE) {
          is_done = true;
        }
        else {
          is_done = true;
          error = true;
        }
      } break;
      case JSON_EXPECT_TYPE_COLON: {
        if ( token.type == JSON_TOKEN_TYPE_COLON ) {
          expect_type = JSON_EXPECT_TYPE_VALUE; 
        }
        else {
          is_done = true;
          error = true;
        }
      } break;
      case JSON_EXPECT_TYPE_VALUE: {
        if (token.type == JSON_TOKEN_TYPE_UNSIGNED_INTEGER) {
          U32 number = 0;
          B32 success = str8_to_u32_range(j->text, token.begin, token.ope, &number);
          if (success) {
            current_node->value_u32 = number;
            current_node->value_type = JSON_VALUE_TYPE_U32;
            json_insert_node(&root, current_node);
          }
          else {
            is_done = true;
            error = true;
          }
        }
        else if(token.type == JSON_TOKEN_TYPE_SIGNED_INTEGER) {

          S32 number = 0;
          B32 success = str8_to_s32_range(j->text, token.begin, token.ope, &number);
          if (success) {

          printf("Hello");
            current_node->value_s32 = number;
            current_node->value_type = JSON_VALUE_TYPE_S32;
            json_insert_node(&root, current_node);
          }
          else {
            is_done = true;
            error = true;
          }

        }

        else if(token.type == JSON_TOKEN_TYPE_STRING) 
        {
          U32 token_len = token.ope - token.begin;
          U8* val_mem = (U8*)malloc(token_len);
          for(U32 token_i = 0; token_i < token_len; ++token_i) {
            val_mem[token_i] = j->text.e[token.begin + token_i];
          }
          current_node->value_str8 = str8(val_mem, token_len);
          current_node->value_type = JSON_VALUE_TYPE_STR8; 

          json_insert_node(&root, current_node);
        }
        else if (token.type == JSON_TOKEN_TYPE_NULL) {
          current_node->value_type = JSON_VALUE_TYPE_NULL;
          json_insert_node(&root, current_node);
        }
        else if (token.type == JSON_TOKEN_TYPE_OPEN_BRACE) {
          Json_Node* node = json_parse_object(j); 
          current_node->value_obj = node;
          current_node->value_type = JSON_VALUE_TYPE_OBJECT;
          json_insert_node(&root, current_node);
        }
        else {
          is_done = true;
          error = true;
        }
        // TODO: array
        // TODO
        expect_type = JSON_EXPECT_TYPE_COMMA_OR_CLOSE;

    
      } break;
      case JSON_EXPECT_TYPE_COMMA_OR_CLOSE:{
        if (token.type == JSON_TOKEN_TYPE_COMMA) {
          expect_type = JSON_EXPECT_TYPE_KEY_OR_CLOSE;
        }
        if (token.type == JSON_TOKEN_TYPE_CLOSE_BRACE) {
          is_done = true; //is this right?? 
        }


      } break;
    }
  }

  return root;
}

static U32 scope = 0;
static void 
json_print_nodes_in_order(Json_Node* root) {

  if (root == null) {
    return;
  }
  else {
    json_print_nodes_in_order(root->left);
    for(U32 _i = 0; _i < scope; ++_i) 
      printf(" ");

    for (U32 i = 0; i < root->key.count; ++i)
      printf("%c", root->key.e[i]);
    printf(":");
    switch(root->value_type) {
      case JSON_VALUE_TYPE_STR8: {
        for (U32 i = 0; i < root->value_str8.count; ++i) {
          printf("%c", root->value_str8.e[i]);
        }
      } break;
      case JSON_VALUE_TYPE_U32: {
        printf("%d", root->value_u32);
      } break;
      case JSON_VALUE_TYPE_S32: {
        printf("%d", root->value_s32);
      } break;

      case JSON_VALUE_TYPE_NULL: {
        printf("null");
      } break;
      case JSON_VALUE_TYPE_OBJECT: {
        scope++;
        printf("{\n");
        json_print_nodes_in_order(root->value_obj);

        scope--;
        for(U32 _i = 0; _i < scope; ++_i) 
          printf(" ");

        printf("}");
        break;
      }
    }
    
    printf("\n");

    json_print_nodes_in_order(root->right);
  }
}


int main() {
  Json_Tokenizer j = json_read_for_tokenizer("test_json.json");
 
  U32 scope_level = 0;
  B32 is_done = false;
  Json_Expect_Type expect_type = JSON_EXPECT_TYPE_OPEN; 

  Json_Token token = json_next_token(&j);
  if (token.type != JSON_TOKEN_TYPE_OPEN_BRACE) return 1;
  
  Json_Node* root = json_parse_object(&j);

  // print the node in order
  printf("=== Printing json tree in-order ===\n");
  json_print_nodes_in_order(root);

}




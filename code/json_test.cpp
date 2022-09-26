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
  
  
  JSON_TOKEN_TYPE_IDENTIFIER,
  JSON_TOKEN_TYPE_STRING,
  JSON_TOKEN_TYPE_NUMBER,
  
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

      // TODO: we need to constrain this to identifiers we accept
      if(is_alpha(t->text[t->at]) ||
         is_digit(t->text[t->at]))
      {

        while(is_alpha(t->text[t->at]) ||
              is_digit(t->text[t->at]) ||
              t->text[t->at] == '_') 
        {
          ++t->at;
        }
        ret.ope = t->at;
        ret.type = JSON_TOKEN_TYPE_IDENTIFIER;
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
    printf("%c", t->text[i]);
  }
}

typedef enum  {
  JSON_VALUE_TYPE_NUMBER,
  JSON_VALUE_TYPE_BOOLEAN,
  JSON_VALUE_TYPE_ARRAY,
  JSON_VALUE_TYPE_STRING,
  JSON_VALUE_TYPE_OBJECT,
  JSON_VALUE_TYPE_NULL
} Json_Value_Type;

typedef struct Json_Node{
  String8 key;

  Json_Value_Type value_type;
  union {
    String8 value_str;
    F32 value_number;
    Json_Node* value_object;
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
    key_mem[i] = j->text[key.begin + i];
  }
  node->key = str8(key_mem, key_len);  
}

static void
json_set_node_value_string(Json_Node* node, Json_Tokenizer* j, Json_Token val) {
  // TODO: this is hacky and just for testing
  U32 val_len = val.ope - val.begin;;

  U8* val_mem = (U8*)malloc(val_len);
  for(U32 i = 0; i < val_len; ++i) {
    val_mem[i] = j->text[val.begin + i];
  }
  
  // TODO: evaluate value correctly?
  node->value_str = str8(val_mem, val_len);
  node->value_type = JSON_VALUE_TYPE_STRING; 
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
#if 0
          printf("key found: "); 
          json_print_token(j, token);
          printf("\n");
#endif
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
        if(token.type == JSON_TOKEN_TYPE_STRING ||
           token.type == JSON_TOKEN_TYPE_IDENTIFIER) 
        {
#if 0
          printf("value found: "); 
          json_print_token(j, token);
          printf("\n");
#endif
          
          // Ok we try to make a node
          json_set_node_value_string(current_node, j, token);
          json_insert_node(&root, current_node);
        }
        else if (token.type == JSON_TOKEN_TYPE_OPEN_BRACE) {
          printf("object found\n");
          Json_Node* node = json_parse_object(j); 
          current_node->value_object = node;
          json_insert_node(&root, current_node);
        }
        
        expect_type = JSON_EXPECT_TYPE_COMMA_OR_CLOSE;

        // TODO: array
    
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

static void 
json_print_nodes_in_order(Json_Node* root) {
  if (root == null) {
    return;
  }
  else {
    json_print_nodes_in_order(root->left);
    for (U32 i = 0; i < root->key.count; ++i)
      printf("%c", root->key.e[i]);
    printf(":");
    for (U32 i = 0; i < root->value_str.count; ++i)
      printf("%c", root->value_str.e[i]);

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
  //json_print_nodes_in_order(root);

}




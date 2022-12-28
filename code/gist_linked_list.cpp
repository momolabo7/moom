// This is a code for a 'performant' circlular double linked list
// that uses a sentinel to avoid branching

#include "momo.h"
#include <stdio.h>


struct gist_cdl_node {
  int value;

  gist_cdl_node* prev;
  gist_cdl_node* next;
};


static gist_cdl_node nodes[128];
static u32_t node_count = 0;


static gist_cdl_node* 
gist_cdl_new_node(int value) {
  gist_cdl_node* node = nodes + node_count++;
  node->value = value;

  return node;
}


static void
gist_cdl_init(gist_cdl_node* sentinel) {
  sentinel->next = sentinel;
  sentinel->prev = sentinel;

}
static void
gist_cdl_push_back(gist_cdl_node* sentinel, int value) {
  gist_cdl_node* new_node = gist_cdl_new_node(value);
  
  new_node->next = sentinel;
  new_node->prev = sentinel->prev;
  new_node->prev->next = new_node;
  new_node->next->prev = new_node;
}

static void
gist_cdl_push_front(gist_cdl_node* sentinel, int value) {
  gist_cdl_node* new_node = gist_cdl_new_node(value);
  
  
  new_node->prev = sentinel;
  new_node->next = sentinel->next;
  new_node->next->prev = new_node;
  new_node->prev->next = new_node;


}

static void
gist_cdl_print_forward(gist_cdl_node* sentinel) {
  gist_cdl_node* itr = sentinel->next;
  while(itr != sentinel) {
    printf("%d ", itr->value);
    itr = itr->next;
  }
}

static void
gist_cdl_print_backwards(gist_cdl_node* sentinel) {
  gist_cdl_node* itr = sentinel->prev;
  while(itr != sentinel) {
    printf("%d ", itr->value);
    itr = itr->prev;
  }
}


int main() {
  gist_cdl_node sentinel;
  gist_cdl_init(&sentinel);

  gist_cdl_push_back(&sentinel, 1);
  gist_cdl_push_back(&sentinel, 2);
  gist_cdl_push_back(&sentinel, 3);

  // should print 1 2 3 3 2 1
  gist_cdl_print_forward(&sentinel);
  gist_cdl_print_backwards(&sentinel);
  printf("\n");

  gist_cdl_push_front(&sentinel, 4);
  gist_cdl_push_front(&sentinel, 5);
  gist_cdl_push_front(&sentinel, 6);

  // should print 654123
  gist_cdl_print_forward(&sentinel);

}


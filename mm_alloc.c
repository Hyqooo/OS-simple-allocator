/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines.
 */

#include "mm_alloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct a_block{
  // metadata
  struct a_block *prev;
  struct a_block *next;
  bool free;
  size_t size;

  // Allocated memory
  // So, we use char because it always has size of 1 byte, and we can easily allocate needed amount of memory
  char memory[];
}allocated_block_t;

// List of all allocated memory
allocated_block_t *allocated_memory = NULL;

// NOTE: I do not zero-fill newly allocated memory
void *mm_malloc(size_t size) {
  if (size == 0)
    return NULL;

  // It could be written better
  // but until I don't understand how to write it better
  // I can't do anything 
  if (allocated_memory == NULL){
    allocated_block_t *new_block = sbrk(sizeof(allocated_block_t) + size);
    if (new_block == (void *)-1) return NULL;
    new_block->size = size;
    new_block->free = false;
    allocated_memory = new_block;
    return allocated_memory->memory;
  }
  allocated_block_t *cur;

  // go through allocated memory and try to find first block that fits requested data
  for (cur = allocated_memory; cur->next != NULL; cur = cur->next){
    if (cur->free && cur->size >= size){
      size_t old_size = cur->size;
      cur->free = false;
      cur->size = size;
      
      // if there's space for another metadata header
      if ((old_size - cur->size) >= sizeof(allocated_block_t)){
        allocated_block_t *new_block = cur + cur->size;
        new_block->size = old_size - cur->size;
        new_block->free = false;
        new_block->prev = cur;
        new_block->next = cur->next;
        cur->next = new_block;
      }
      return cur->memory; 
    }  
  }

  allocated_block_t *new_block = sbrk(sizeof(allocated_block_t) + size);
  if (new_block == (void *)-1) return NULL;
  new_block->size = size;
  new_block->prev = cur;
  cur->next = new_block;
  new_block->next = NULL;
  new_block->free = false;

  return new_block->memory;
}

void *mm_realloc(void *ptr, size_t size) {
  /* YOUR CODE HERE */
  return NULL;
}

void merge(allocated_block_t *block){
  allocated_block_t *next_block = block->next;

  block->size = block->size + next_block->size + sizeof(allocated_block_t);
  block->next = next_block->next;
  if (next_block->next != NULL)
    next_block->next->prev = block;
  block->free = true;
}

// Yes, if in the list a lot of blocks we need go through whole list each time
// and it's increases complexity to O(n) but this is really what I want
void merge_free_blocks(){
  allocated_block_t *cur;
  
  for (cur = allocated_memory; cur->next != NULL; cur = cur->next){
    if (cur->free && cur->next->free){
      merge(cur);
      if (cur->next == NULL)
        return;
    }
  } 
}

// if there are  two blocks before and after of block to be released
// I need to coalesce them
void mm_free(void *ptr) {
  if (!ptr) return;

  allocated_block_t *meta = ptr - sizeof(allocated_block_t);

  meta->free = true;
  merge_free_blocks();
}


void print_list(int number){
  allocated_block_t *cur;
  int i;

  printf("Try #%d ****\n", number);

  for(cur = allocated_memory, i = 0; cur != NULL; cur = cur->next, i++){
    printf("Block #%d - %p\n", i, cur);
  }
}

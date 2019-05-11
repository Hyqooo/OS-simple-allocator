/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines.
 */

#include "mm_alloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
// debug
#include <stdio.h>


typedef struct a_block{
  // metadata
  struct a_block *prev;
  struct a_block *next;
  bool free;
  size_t size;

  // Allocated memory
  // So, we use char because it always has size of 1 byte, and we can easily allocate needed amount of memory
  char memory[0];
}allocated_block_t;

#define META_SIZE sizeof(allocated_block_t)

// List of all allocated memory
allocated_block_t *allocated_memory = NULL;
allocated_block_t *tail_of_list = NULL;

void add_to_list(allocated_block_t *prev, allocated_block_t *new_block) {
  new_block->prev = prev;
  if (prev != NULL){
    new_block->next = prev->next;
    prev->next = new_block;
  }
  tail_of_list = new_block;
  if (allocated_memory == NULL)
    allocated_memory = tail_of_list;
}

void form_new_block(allocated_block_t *block, size_t size, bool is_free) {
  block->size = size;
  block->free = is_free;
} 

void *mm_malloc(size_t size) {
  if (size == 0) return NULL;

  for (allocated_block_t *cur = allocated_memory; cur != NULL; cur = cur->next) {
    if (cur->free && cur->size >= size) {
      size_t old_size = cur->size;
      form_new_block(cur, size, false);

      // there's space for another header and data
      if ((old_size - cur->size) > META_SIZE) {
        allocated_block_t *new_block = cur + META_SIZE + cur->size;
        form_new_block(new_block, old_size - cur->size - META_SIZE, true);
        add_to_list(cur, new_block);
      }
      return cur->memory;
    }
  }
  
  // haven't found free block
  allocated_block_t *new_block = sbrk(META_SIZE + size);
  form_new_block(new_block, size, false);
  add_to_list(tail_of_list, new_block);
  return new_block->memory;
}

void *mm_realloc(void *ptr, size_t size) {
  
  // first we need to free referenced block
  // then we need to mm_alloc of specified size
    
  
  
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
    printf("Block #%d - %p - Is free: %d - size: %ld - size w/ meta: %ld\n", i, cur, cur->free, cur->size, cur->size + sizeof(allocated_block_t));
  }
}

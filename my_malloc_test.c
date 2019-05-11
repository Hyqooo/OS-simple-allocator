#include <assert.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

/* Function pointers to hw3 functions */
void* (*mm_malloc)(size_t);
void* (*mm_realloc)(void*, size_t);
void (*mm_free)(void*);
void (*print_list)(int);

void load_alloc_functions() {
    void *handle = dlopen("hw3lib.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    char* error;
    mm_malloc = dlsym(handle, "mm_malloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_realloc = dlsym(handle, "mm_realloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_free = dlsym(handle, "mm_free");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    print_list = dlsym(handle, "print_list");
    if ((error = dlerror()) != NULL) {
      fprintf(stderr, "%s\n", dlerror());
      exit(1);
    }
}


void alloc_and_free(){
  int *data = mm_malloc(sizeof(int));
  print_list(1);
  int *data2 = mm_malloc(sizeof(int));
  print_list(2);
  int *data3 = mm_malloc(sizeof(int));
  print_list(3);

  mm_free(data2);
  print_list(4);
  mm_free(data3);
  print_list(5);

  char *var = mm_malloc(sizeof(char) * 12);
  print_list(6);

  mm_free(data);
  print_list(7);
  mm_free(var);
  print_list(8);
}


// allocate block of certain size, free it, allocate block of equal or less size
// BUG: it is not working correctly
void test_2(){
  int *data = (int*)mm_malloc(sizeof(int) * 20);
  print_list(1);

  mm_free(data);

  int *data2 = (int*)mm_malloc(sizeof(int) * 5);
  print_list(2);
}

int main(){
  load_alloc_functions();
  
  alloc_and_free();
 //  test_2();

  printf("Successfull\n");
  return 0;
}

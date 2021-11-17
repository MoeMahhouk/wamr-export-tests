#include <stdio.h>
#include <stdlib.h> 

#define import(name) __attribute__((import_name(name)))
#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))



import("get_key_value")
extern char* get_key_value(int index);

import("set_key_value")
extern void set_key_value(int index, char *value);

export("module_malloc")
char *module_malloc(int size){
    if(size <= 0) return NULL;
    char *result = (char*) malloc(size);
    return result==NULL? NULL: result;
}

export("main")  
int main(){

  char *result;
  for(int i= 0; i<BENCH_ITERATIONS; i++){
    result = get_key_value(0);
    //printf("getting index 0 is %s\n", result);
    free(result);
  }
  return 0;
}

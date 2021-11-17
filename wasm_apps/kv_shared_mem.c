#include <stdio.h>
#include <stdlib.h> 

#define import(name) __attribute__((import_name(name)))
#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))

char *shared_mem=NULL;

import("get_key_value")
extern int get_key_value(int index);

import("set_key_value")
extern void set_key_value(int index, char *value);

export("init_shared_mem")
char *init_shared_mem(int size){
    if(size <= 0) return NULL;
    shared_mem = (char*) malloc(size);
    return shared_mem==NULL? NULL: shared_mem;
}

export("main")  
int main(){

  //set_key_value(0, "ok");
  int value_length;
  for(int i= 0; i<BENCH_ITERATIONS; i++){
    value_length = get_key_value(0);
    //printf("getting index 0 is %.*s\n", value_length, shared_mem);
  }
  return 0;
}

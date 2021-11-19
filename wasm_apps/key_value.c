#include <stdio.h>
#include <stdlib.h> 

#define import(name) __attribute__((import_name(name)))
#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))



import("get_key_value")
extern char* get_key_value(int index);

import("set_key_value")
extern void set_key_value(int index, char *value);

import("native_module_free")
extern void native_module_free(void *ptr);

export("module_malloc")
char *module_malloc(int size){
    if(size <= 0) return NULL;
    char *result = (char*) malloc(size);
    return result==NULL? NULL: result;
}


export("main")  
int main(){


  set_key_value(0, "moeeeee");
  set_key_value(1, "kai");
  set_key_value(2, "ok");
    
  char *result = get_key_value(0);
  char *result2 = get_key_value(1);
  char *result3 = get_key_value(2);
  printf("getting index 0 is %s\n", result);
  printf("getting index 1 is %s\n", result2);
  printf("getting index 2 is %s\n", result3);
  
  #if defined VARIANT && VARIANT == 2
  /* Variant 2 */
  native_module_free(result);
  native_module_free(result2);
  native_module_free(result3);
  printf("getting index 0 after free is %s\n", result);
  printf("getting index 1 after free is %s\n", result2);
  printf("getting index 2 after free is %s\n", result3);
  #endif

  /* Variant 3    */
  /*free(result);
  free(result2);
  free(result3);*/
  return 0;
}

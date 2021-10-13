//#include <stdio.h>
#define import(name) __attribute__((import_name(name)))
#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))

import("my_printf")
extern void my_printf(char *text);

import("my_nprintf")
extern void my_nprintf(char *text, int len);

import("hello_world")
extern void hello_world();

export("main_test")
int main(){
  //printf("executing wasi printf\n");
  hello_world();
  my_printf("calling now my_printf imported function");
  char *hello = "hello there";  
  my_nprintf(hello, 12);
  return 0;
}

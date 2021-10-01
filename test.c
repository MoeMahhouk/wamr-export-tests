#include <stdio.h>
#define import(name) __attribute__((import_name(name)))

import("my_printf")
extern void my_printf(char *text);

import("my_nprintf")
extern void my_nprintf(char *text, size_t len);

import("hello_world")
extern void hello_world();


int main (){
  hello_world();
  my_printf("calling now my_printf imported function");
  char *hello = "hello there";  
  my_nprintf(hello, 12);
}

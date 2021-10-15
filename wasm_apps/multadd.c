#include <stdio.h>
#define import(name) __attribute__((import_name(name)))
#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))

__attribute__((import_module("add.wasm"))) import("addFunc")
extern int addFunc(int a, int b);

export("multadd")
int multadd(int a, int b, int c)
{
  return addFunc(a,b) * c;
}

int main()
{
  int res = multadd(1,1,2);
  printf("1+1 is 2 * 2 is %d quick math!! \n", res);
  return res;
}
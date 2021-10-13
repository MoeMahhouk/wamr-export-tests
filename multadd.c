#define import(name) __attribute__((import_name(name)))
#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))

import("addFunc") 
extern int addFunc(int a, int b);

export("multadd")
int multadd(int a, int b, int c)
{
  return addFunc(a,b) * c;
}
int main()
{
  return 0;
}
#include <stdio.h>
#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))


int sum=10;
export("addFunc")
int addFunc(int a, int b){
  sum += a+b;
  return sum;
}

export("check")
void check()
{
  sum+= 2;
  printf("sum is equal to %d\n", sum);
}

export("main")
int main()
{
  int result = addFunc(1,1);
  printf("1 + 1 is %d quick quick quick math\n", result);
  return result;
}
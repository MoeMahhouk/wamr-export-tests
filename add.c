#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))

export("addFunc")
int addFunc(int a, int b){
  return a+b;
}

int main()
{
  int result = addFunc(1,1);
  printf("1 + 1 is %d quick math\n", result);
  return result;
}
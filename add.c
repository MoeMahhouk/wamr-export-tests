#define export(name) __attribute__((export_name(name))) __attribute__((visibility("default")))

export("addFunc")
int addFunc(int a, int b){
  return a+b;
}

int main()
{
  return 0;
}
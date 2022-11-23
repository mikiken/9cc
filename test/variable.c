int printf();

int assert();

int local_variable_test1() {
  int a;
  a = 2;
  return a;
}

int local_variable_test2() {
  int x;
  x = 4;
  int y;
  y = 2;
  return x + y;
}

int local_variable_test3() {
  int p;
  int q;
  int r;
  p = q = 3;
  r = 2;
  return p + q * r;
}

int local_variable_test4() {
  int foo;
  foo = 5;
  return foo;
}

int local_variable_test5() {
  int bar;
  bar = 2;
  return bar * 3 + 2;
}

int local_variable_test6() {
  int _foo123;
  int bar;
  _foo123 = 4;
  bar = 2;
  return _foo123 + bar;
}

int local_variable_test7() {
  int Foo_123_bar;
  Foo_123_bar = 3;
  return (-Foo_123_bar + 5) * 2;
}

int global_variable1;
int global_variable2[20];
int *global_variable3;
int *global_variable4[2];

int global_variable_test1() {
  return global_variable1;
}

int global_variable_test2() {
  return global_variable2[2];
}

int global_variable_test3() {
  global_variable1 = 3;
  global_variable2[2] = 2;
  return global_variable1 + global_variable2[2];
}

int global_variable_test4() {
  int x;
  global_variable3 = &x;
  x = 3;
  return *global_variable3;
}

int global_variable_test5() {
  int x;
  int y;
  x = 7;
  y = 1;
  global_variable4[0] = &x;
  global_variable4[1] = &y;
  return *global_variable4[0] + *global_variable4[1];
}

void variable_test() {
  // ローカル変数
  assert(2, local_variable_test1(), "{int a; a=2; return a;}");
  assert(6, local_variable_test2(), "{int x; x=4; int y; y=2; return x+y;}");
  assert(9, local_variable_test3(), "{int p; int q; int r; p=q=3; r=2; return p+q*r;}");
  assert(5, local_variable_test4(), "{int foo; foo=5; return foo;}");
  assert(8, local_variable_test5(), "{int bar; bar=2; return bar*3+2;}");
  assert(6, local_variable_test6(), "{int _foo123; int bar; _foo123=4; bar=2; return _foo123+bar;}");
  assert(4, local_variable_test7(), "{int Foo_123_bar; Foo_123_bar=3; return (-Foo_123_bar+5)*2;}");

  // グローバル変数
  assert(0, global_variable_test1(), "global_variable_test1()");
  assert(0, global_variable_test2(), "global_variable_test2()");
  assert(5, global_variable_test3(), "global_variable_test3()");
  assert(3, global_variable_test4(), "global_variable_test4()");
  assert(8, global_variable_test5(), "global_variable_test5()");
  assert(4, sizeof(global_variable1), "sizeof(global_variable1)");
  assert(80, sizeof(global_variable2), "sizeof(global_variable2)");

  printf("All variable test cases have passed.\n\n");
}
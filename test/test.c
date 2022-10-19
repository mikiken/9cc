int printf();
int exit();

int assert(int expected, int actual, char *code) {
  if (expected == actual) {
    printf("%s => %d\n", code, actual);
  }
  else {
    printf("%s => %d expected, but got %d\n", code, expected, actual);
    exit(1);
  }
  return 0;
}

/**
 * block comment test
 * 
 */

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

int foo() {
  return 5;
}

int add2(int a, int b) {
  return a + b;
}

int add6(int a, int b, int c, int d, int e, int f) {
  return a + b + c + d + e + f;
}

int sub2(int a, int b) {
  return a - b;
}

int ret3() {
  return 3;
}

int fact(int n) {
  if (n == 1)
    return 1;
  return n * fact(n - 1);
}

int fib(int n) {
  if (n == 1) {
    return 1;
  }
  if (n == 2) {
    return 1;
  }
  return fib(n - 1) + fib(n - 2);
}

int combi(int n, int r) {
  if (r == 0)
    return 1;
  else if (n == r)
    return 1;
  else
    return combi(n - 1, r - 1) + combi(n - 1, r);
}

int sizeof_test1() {
  int x;
  return sizeof(x);
}

int sizeof_test2() {
  int x;
  return sizeof(x + 2);
}

int sizeof_test3() {
  int x;
  return sizeof(x = 2);
}

int sizeof_test4() {
  int x;
  return sizeof x;
}

int sizeof_test5() {
  int *x;
  return sizeof(x);
}

int sizeof_test6() {
  int x;
  return sizeof(&x);
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

int main() {
  // 整数1つを返す
  assert(0, 0, "0");
  assert(42, 42, "42"); // コメント

  // '+','-' 演算子
  assert(21, 5 + 20 - 4, "5 + 20 - 4");
  assert(41, 12 + 34 - 5, "12 + 34 - 5");
  assert(148, 100 + 56 - 8, "100 + 56 - 8");

  // '*','/' 演算子
  assert(47, 5 + 6 * 7, "5+6*7");
  assert(15, 5 * (9 - 6), "5*(9-6)");
  assert(4, (3 + 5) / 2, "(3+5)/2");

  // 単項'+','-'演算子
  assert(10, -10 + 20, "-10+20");
  assert(10, +(+10), "+(+10)");
  assert(10, -(-10), "-(-10)");
  assert(10, (-(-15 + 25) + 50) / 4, "(-(-15 + 25) + 50) / 4");

  // '==','!=' 演算子
  assert(0, 0 == 1, "0==1");
  assert(1, 42 == 42, "42==42");
  assert(1, 0 != 1, "0!=1");
  assert(0, 42 != 42, "42!=42");

  // '<','<=' 演算子
  assert(1, 0 < 1, "0<1");
  assert(0, 1 < 1, "1<1");
  assert(0, 2 < 1, "2<1");
  assert(1, 0 <= 1, "0<=1");
  assert(1, 1 <= 1, "1<=1");
  assert(0, 2 <= 1, "2<=1");

  // '>','>=' 演算子
  assert(1, 1 > 0, "1>0");
  assert(0, 1 > 1, "1>1");
  assert(0, 1 > 2, "1>2");
  assert(1, 1 >= 0, "1>=0");
  assert(1, 1 >= 1, "1>=1");
  assert(0, 1 >= 2, "1>=2");

  // ローカル変数
  assert(2, local_variable_test1(), "{int a; a=2; return a;}");
  assert(6, local_variable_test2(), "{int x; x=4; int y; y=2; return x+y;}");
  assert(9, local_variable_test3(), "{int p; int q; int r; p=q=3; r=2; return p+q*r;}");
  assert(5, local_variable_test4(), "{int foo; foo=5; return foo;}");
  assert(8, local_variable_test5(), "{int bar; bar=2; return bar*3+2;}");
  assert(6, local_variable_test6(), "{int _foo123; int bar; _foo123=4; bar=2; return _foo123+bar;}");
  assert(4, local_variable_test7(), "{int Foo_123_bar; Foo_123_bar=3; return (-Foo_123_bar+5)*2;}");

  // 関数
  assert(5, foo(), "foo()");
  assert(8, foo() + 3, "foo() + 3");
  assert(7, add2(2, 5), "add2(2, 5)");
  assert(3, sub2(10, 7), "sub2(10, 7)");
  assert(21, add6(1, 2, 3, 4, 5, 6), "add6(1,2,3,4,5,6)");
  assert(21, add6(1, 2, 3, 4, 5, add2(2, 4)), "add6(1,2,3,4,5,add2(2,4))");
  assert(66, add6(add6(1, 2, 3, add2(1, 3), 5, 6), 7, 8, 9, 10, 11), "add6(add6(1,2,3,add2(1,3),5,6),7,8,9,10,11)");
  assert(5, ret3() + 2, "ret3()+2");
  assert(24, fact(4), "fact(4)");
  assert(55, fib(10), "fib(10)");
  assert(15, combi(6, 2), "combi(6,2)");

  // sizeof演算子
  assert(4, sizeof_test1(), "{int x; return sizeof(x);}");
  assert(4, sizeof_test2(), "{int x; return sizeof(x+2);}");
  assert(4, sizeof_test3(), "{int x; return sizeof(x=2);}");
  assert(4, sizeof_test4(), "{int x; return sizeof x;}");
  assert(8, sizeof_test5(), "{int *x; return sizeof(x);}");
  assert(8, sizeof_test6(), "{int x; return sizeof(&x);}");

  // グローバル変数
  assert(0, global_variable_test1(), "global_variable_test1()");
  assert(0, global_variable_test2(), "global_variable_test2()");
  assert(5, global_variable_test3(), "global_variable_test3()");
  assert(3, global_variable_test4(), "global_variable_test4()");
  assert(8, global_variable_test5(), "global_variable_test5()");
  assert(4, sizeof(global_variable1), "sizeof(global_variable1)");
  assert(80, sizeof(global_variable2), "sizeof(global_variable2)");

  printf("OK\n");
  return 0;
}
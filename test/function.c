int printf();

int assert(int expected, int actual, char *code);

void func_declaration_test1(void (*funcptr)(int a));

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

int fib1(int n) {
  if (n == 1) {
    return 1;
  }
  if (n == 2) {
    return 1;
  }
  return fib1(n - 1) + fib1(n - 2);
}

int combi(int n, int r) {
  if (r == 0)
    return 1;
  else if (n == r)
    return 1;
  else
    return combi(n - 1, r - 1) + combi(n - 1, r);
}

void void_test1(void) {
  printf("This is a test case of void type.\n");
}

void void_test2(int x) {
  if (x >= 2) {
    printf("Argument x is equal to or greater than 2.\n");
    return;
  }
  else
    printf("Argument x is less than 2.\n");
}

int function_test() {
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
  assert(55, fib1(10), "fib1(10)");
  assert(15, combi(6, 2), "combi(6,2)");
  void_test1();  // => This is a test case of void type.
  void_test2(3); // => Argument x is equal to or greater than 2.
  void_test2(1); // => Argument x is less than 2.

  printf("All function test cases have passed.\n\n");
  return 0;
}

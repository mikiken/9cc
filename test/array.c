int printf();

int assert();

int array_test1() {
  int a[2];
  int *p;
  p = a;
  *p = 1;
  *(p + 1) = 2;
  return *p + *(p + 1);
}

int array_test2() {
  int a;
  a = 2;
  int b[2];
  int c;
  c = 3;
  int *p;
  p = b;
  *p = 1;
  *(p + 1) = 2;
  return a + *p + *(p + 1) + c;
}

int array_test3() {
  int a[2];
  *a = 2;
  *(a + 1) = 3;
  return *a + *(a + 1);
}

int array_test4() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  int *p;
  p = a;
  return *p + *(p + 1);
}

int array_test5() {
  int a[2];
  *a = 1;
  *(1 + a) = 2;
  int *p;
  p = a;
  return *p + *(1 + p);
}

int array_test6() {
  int a[3];
  return sizeof(a);
}

int array_test7() {
  int *a[3];
  return sizeof(a);
}

int array_test8() {
  int a[2];
  a[0] = 2;
  a[1] = 3;
  return a[0] + a[1];
}

int array_test9() {
  int a[2];
  0 [a] = 2;
  1 [a] = 3;
  return 0 [a] + 1 [a];
}

int array_test10() {
  int num;
  num = 0;
  int a[2];
  a[0] = 1;
  a[num + 1] = 2;
  int *p;
  p = a;
  return *p + *(p + 1);
}

int array_test11() {
  int a[2];
  a[0] = 1;
  a[a[0]] = 6;
  int *p;
  p = a;
  return *p + *(p + 1);
}

int array_test12() {
  int a;
  a = 4;
  int b[2];
  int *p;
  p = b;
  *p = 1;
  *(p + 1) = 3;
  return a;
}

int array_test13() {
  int a[5] = {1, 2, 3, 4, 5};
  int sum = 0;
  for (int i = 0; i < 5; i++)
    sum += a[i];
  return sum;
}

int array_test14() {
  int a[5] = {
      3,
      4,
      5,
      6,
      7,
  };
  int sum = 0;
  for (int i = 0; i < 5; i++)
    sum += a[i];
  return sum;
}

/*
int array_test15() {
  int arr[] = {1, 2, 3, 4, 5};
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += arr[i];
  }
  return sum * sizeof(arr) / 6;
}
*/

int array_test16() {
  int x[5] = {1, 2, 3};
  return x[3] + x[4];
}

int fib2(int n) {
  int fib[10];
  fib[0] = 0;
  fib[1] = 1;
  int i;
  for (i = 2; i < 10; i = i + 1) {
    fib[i] = fib[i - 1] + fib[i - 2];
  }
  return fib[n];
}

/*
int array_test17() {
  int arr[3][5];
  arr[2][4] = 3;
  return arr[2][4];
}
*/

void array_test() {
  // 1次元配列
  assert(3, array_test1(), "{int a[2]; int *p; p = a; *p = 1; *(p+1) = 2; return *p + *(p+1);}");
  assert(8, array_test2(), "{int a; a = 2; int b[2]; int c; c = 3; int *p; p = b; *p = 1; *(p+1) = 2; return a + *p + *(p+1) + c;}");
  assert(5, array_test3(), "{int a[2]; *a = 2; *(a+1) = 3; return *a + *(a+1);}");
  assert(3, array_test4(), "{int a[2]; *a = 1; *(a+1) = 2; int *p; p = a; return *p + *(p+1);}");
  assert(3, array_test5(), "{int a[2]; *a = 1; *(1+a) = 2; int *p; p = a; return *p + *(1+p);}");
  assert(12, array_test6(), "{int a[3]; return sizeof(a);}");
  assert(24, array_test7(), "{int *a[3]; return sizeof(a);}");
  assert(5, array_test8(), "{int a[2]; a[0] = 2; a[1] = 3; return a[0] + a[1];}");
  assert(5, array_test9(), "{int a[2]; 0[a] = 2; 1[a] = 3; return 0[a] + 1[a];}");
  assert(3, array_test10(), "{int num; num = 0; int a[2]; a[0] = 1; a[num+1] = 2; int *p; p = a; return *p + *(p+1);}");
  assert(7, array_test11(), "{int a[2]; a[0] = 1; a[a[0]] = 6; int *p; p = a; return *p + *(p+1);}");
  assert(4, array_test12(), "{int a; a = 4; int b[2]; int *p; p = b; *p = 1; *(p+1) = 3; return a;}");
  assert(21, fib2(8), "fib(8)");

  // 初期化式
  assert(15, array_test13(), "{int a[5] = {1, 2, 3, 4, 5}; int sum = 0; for (int i = 0; i < 5; i++) sum += a[i]; return sum;}");
  assert(25, array_test14(), "{int a[5] = {3, 4, 5, 6, 7,}; int sum = 0; for (int i = 0; i < 5; i++) sum += a[i]; return sum;}");
  // assert(50, array_test15(), "{int arr[] = {1, 2, 3, 4, 5}; int sum = 0; for (int i = 0; i < 5; i++) sum += arr[i]; return sum * sizeof(arr) / 6;}");
  assert(0, array_test16(), "{int x[5] = {1, 2, 3}; return x[3] + x[4];}");

  // 多次元配列
  //assert(21, array_test17(), "{int arr[3][5]; arr[2][4] = 3; return arr[2][4];}");

  printf("\x1b[32m"); // 文字色を緑に設定
  printf("All array test cases have passed.\n\n");
  printf("\x1b[0m");
}

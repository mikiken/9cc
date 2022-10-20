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

int if_test1() {
  if (0)
    3;
  return 5;
}

int if_test2() {
  if (1)
    return 8;
}

int if_test3() {
  if (1)
    return 2;
  return 7;
}

int if_test4() {
  if (2 + 3 == 5)
    return 3 * 2;
}

int if_test5() {
  int a;
  a = -1;
  if (a > 0)
    return 5;
  return 3;
}

int if_test6() {
  if (0)
    return 3;
  else
    return 2;
}

int if_test7() {
  if (1)
    return 3;
  else
    return 2;
}

int if_test8() {
  if (0)
    return 3;
  else
    2;
  return 4;
}

int if_test9() {
  int a;
  a = 2 * 3;
  if (a < 5)
    return 3;
  else if (a == 6)
    return 4;
}

int if_test10() {
  int a;
  a = 2 * 3;
  if (a < 5)
    return 3;
  else if (a == 7)
    return 4;
  return 2;
}

int while_test1() {
  while (0)
    return 1;
  return 2;
}

int while_test2() {
  int x;
  x = 0;
  while (x < 5)
    x = x + 1;
  return x;
}

int for_test1() {
  int x;
  int i;
  x = 0;
  for (i = 0; i < 5; i = i + 1)
    x = x + 1;
  return x;
}

int for_test2() {
  for (;;)
    return 3;
}

int control_statement_test1() {
  int a;
  int i;
  a = 0;
  for (i = 0; i < 10; i = i + 1) {
    a = a + 2;
    if (a == 6)
      return a;
  }
}

int control_statement_test2() {
  int a;
  int b;
  int i;
  int j;
  a = 0;
  b = 0;
  for (i = 0; i < 10; i = i + 1) {
    a = a + 1;
    if (i == 9) {
      a = a * 2;
    }
  }
  for (j = 0; j < 10; j = j + 1) {
    b = b + 1;
    if (j == 9) {
      b = b * 2;
    }
  }
  return a + b;
}

int control_statement_test3() {
  int a;
  a = 4;
  if (a == 4) {
    a = a + 2;
    if (a == 6) {
      a = a - 3;
      return a;
    }
  }
}

int control_statement_test4() {
  int sum;
  int i;
  int j;
  sum = 0;
  for (i = 1; i <= 5; i = i + 1) {
    for (j = 1; j <= 5; j = j + 1) {
      sum = sum + 1;
    }
  }
  return sum;
}

int control_statement_test5() {
  int a;
  a = 5;
  if (a >= 0) {
    if (a == 5) {
      return a;
    }
  }
}

int control_statement_test6() {
  int a;
  int b;
  a = 3;
  {
    b = 7;
    if (a == 3)
      return a + b;
  }
}

int control_statement_test7() {
  int a;
  a = 3;
  if (a == 1)
    return 1;
  if (a == 2)
    return 2;
  if (a == 3)
    return 3;
}

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

  // if文
  assert(5, if_test1(), "{if(0) 3; return 5;}");
  assert(8, if_test2(), "{if (1) return 8;}");
  assert(2, if_test3(), "{if(1) return 2; return 7;}");
  assert(6, if_test4(), "{if (2+3==5) return 3*2;}");
  assert(3, if_test5(), "{int a; a=-1; if(a>0) return 5; return 3;}");
  assert(2, if_test6(), "{if(0) return 3; else return 2;}");
  assert(3, if_test7(), "{if(1) return 3; else return 2;}");
  assert(4, if_test8(), "{if(0) return 3; else 2; return 4;}");
  assert(4, if_test9(), "{int a; a=2*3; if(a<5) return 3; else if(a==6) return 4;}");
  assert(2, if_test10(), "{int a; a=2*3; if(a<5) return 3; else if(a==7) return 4; return 2;}");

  // while文
  assert(2, while_test1(), "{while(0) return 1; return 2;}");
  assert(5, while_test2(), "{int x; x=0; while(x<5) x=x+1; return x;}");

  // for文
  assert(5, for_test1(), "{int x; int i; x=0; for(i=0;i<5;i=i+1) x=x+1; return x;}");
  assert(3, for_test2(), "{for (;;) return 3;}");

  // block (compound statement)

  // 制御構文のネスト
  assert(6, control_statement_test1(), "{int a; int i; a=0; for(i=0; i<10; i=i+1){a=a+2; if(a==6) return a;}}");
  assert(40, control_statement_test2(), "{int a; int b; int i; int j; a=0; b=0; for(i=0; i<10; i=i+1){a=a+1; if(i==9){a=a*2;}} for(j=0; j<10; j=j+1){b=b+1; if(j==9){b=b*2;}} return a+b;}");
  assert(3, control_statement_test3(), "{int a; a=4; if(a==4){a=a+2;if(a==6) {a=a-3; return a;}}}");
  assert(25, control_statement_test4(), "{int sum; int i; int j; sum=0; for(i=1;i<=5;i=i+1){for(j=1;j<=5;j=j+1){sum=sum+1;}} return sum;}");
  assert(5, control_statement_test5(), "{int a; a=5; if(a>=0){if(a==5){return a;}}}");
  assert(10, control_statement_test6(), "{int a; int b; a=3; {b=7; if(a==3) return a+b;}}");
  assert(3, control_statement_test7(), "{int a; a=3; if(a==1) return 1; if(a==2) return 2; if(a==3) return 3;}");

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

  // sizeof演算子
  assert(4, sizeof_test1(), "{int x; return sizeof(x);}");
  assert(4, sizeof_test2(), "{int x; return sizeof(x+2);}");
  assert(4, sizeof_test3(), "{int x; return sizeof(x=2);}");
  assert(4, sizeof_test4(), "{int x; return sizeof x;}");
  assert(8, sizeof_test5(), "{int *x; return sizeof(x);}");
  assert(8, sizeof_test6(), "{int x; return sizeof(&x);}");

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
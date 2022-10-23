int printf();

int assert();

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

int control_statement_test() {
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

  // 制御構文のネスト
  assert(6, control_statement_test1(), "{int a; int i; a=0; for(i=0; i<10; i=i+1){a=a+2; if(a==6) return a;}}");
  assert(40, control_statement_test2(), "{int a; int b; int i; int j; a=0; b=0; for(i=0; i<10; i=i+1){a=a+1; if(i==9){a=a*2;}} for(j=0; j<10; j=j+1){b=b+1; if(j==9){b=b*2;}} return a+b;}");
  assert(3, control_statement_test3(), "{int a; a=4; if(a==4){a=a+2;if(a==6) {a=a-3; return a;}}}");
  assert(25, control_statement_test4(), "{int sum; int i; int j; sum=0; for(i=1;i<=5;i=i+1){for(j=1;j<=5;j=j+1){sum=sum+1;}} return sum;}");
  assert(5, control_statement_test5(), "{int a; a=5; if(a>=0){if(a==5){return a;}}}");
  assert(10, control_statement_test6(), "{int a; int b; a=3; {b=7; if(a==3) return a+b;}}");
  assert(3, control_statement_test7(), "{int a; a=3; if(a==1) return 1; if(a==2) return 2; if(a==3) return 3;}");

  printf("All test cases for control statement have passed.\n\n");
  return 0;
}

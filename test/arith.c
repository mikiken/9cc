int printf();

int assert();

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

int arith_test() {
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

  // '%' 演算子
  assert(1, 5 % 2, "5 % 2");
  assert(9, (4 + 5) % 10, "9 % 10");
  assert(0, (3 + 8 * 4) % 7, "(3 + 8 * 4) % 7");

  // sizeof演算子
  assert(4, sizeof_test1(), "{int x; return sizeof(x);}");
  assert(4, sizeof_test2(), "{int x; return sizeof(x+2);}");
  assert(4, sizeof_test3(), "{int x; return sizeof(x=2);}");
  assert(4, sizeof_test4(), "{int x; return sizeof x;}");
  assert(8, sizeof_test5(), "{int *x; return sizeof(x);}");
  assert(8, sizeof_test6(), "{int x; return sizeof(&x);}");

  printf("All arithmetical test cases have passed.\n\n");
  return 0;
}
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

int compound_assignment_operator_test1() {
  int i;
  i = 1;
  i += 1 + 2 * 3;
  return i;
}

int compound_assignment_operator_test2() {
  int i;
  i = 10;
  i -= 3 + 5 % 2;
  return i;
}

int compound_assignment_operator_test3() {
  int i;
  i = 2;
  i *= 3 + 5 % 2;
  return i;
}

int compound_assignment_operator_test4() {
  int i;
  i = 8;
  i /= 3 + 5 % 2;
  return i;
}

int compound_assignment_operator_test5() {
  int i;
  i = 5;
  i %= 3 + 5 % 2;
  return i;
}

int pre_increment_operator_test1() {
  int i;
  i = 1;
  return ++i + 2;
}

int pre_increment_operator_test2() {
  int i;
  i = 2;
  i = ++i;
  return i;
}

int pre_increment_operator_test3() {
  int i;
  int sum;
  sum = 0;
  for (i = 1; i <= 10; ++i) {
    sum += i;
  }
  return sum;
}

int pre_decrement_operator_test1() {
  int i;
  i = 4;
  return --i - 1;
}

int pre_decrement_operator_test2() {
  int i;
  i = 10;
  i = --i;
  return i;
}

int pre_decrement_operator_test3() {
  int i;
  int sum;
  sum = 100;
  for (i = 10; i > 0; --i) {
    sum -= i;
  }
  return sum;
}

int post_increment_operator_test1() {
  int x;
  int i;
  i = 10;
  x = i++;
  return x + i;
}

int post_decrement_operator_test1() {
  int x;
  int i;
  i = 10;
  x = i--;
  return x + i;
}

int not_operator_test1() {
  int x;
  x = 100;
  if (!x)
    return 3;
  else
    return 8;
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

  // '+=' '-=' '*=' '/=' '%=' 演算子
  assert(8, compound_assignment_operator_test1(), "{int i; i = 1; i += 1 + 2 * 3; return i;}");
  assert(6, compound_assignment_operator_test2(), "{int i; i = 10; i -= 3 + 5 % 2; return i;}");
  assert(8, compound_assignment_operator_test3(), "{int i; i = 2; i *= 3 + 5 % 2; return i;}");
  assert(2, compound_assignment_operator_test4(), "{int i; i = 8; i /= 3 + 5 % 2; return i;}");
  assert(1, compound_assignment_operator_test5(), "{int i; i = 5; i %= 3 + 5 % 2; return i;}");

  // 前置 '++' '--' 演算子
  assert(4, pre_increment_operator_test1(), "{int i; i = 1; return ++i + 2;}");
  assert(3, pre_increment_operator_test2(), "{int i; i = 2; i = ++i; return i;}");
  assert(55, pre_increment_operator_test3(), "{int i; int sum; sum = 0; for (i = 1; i <= 10; ++i) { sum += i; } return sum;}");
  assert(2, pre_decrement_operator_test1(), "{int i; i = 4; return --i - 1;}");
  assert(9, pre_decrement_operator_test2(), "{int i; i = 10; i = --i; return i;}");
  assert(45, pre_decrement_operator_test3(), "{int i; int sum; sum = 100; for (i = 10; i > 0; --i) { sum -= i; } return sum;}");

  // 後置 '++' '--' 演算子
  assert(21, post_increment_operator_test1(), "{int x; int i; i = 10; x = i++; return x + i;}");
  assert(19, post_decrement_operator_test1(), "{int x; int i; i = 10; x = i--; return x + i;}");

  // '!' 演算子
  assert(0, !1, "{return !1;}");
  assert(1, !0, "{return !0;}");
  assert(8, not_operator_test1(), "{int x; x = 100; if (!x) return 3; else return 8;}");

  // '&&' '||' 演算子
  assert(0, 0 && 0, "{return 0 && 0;}");
  assert(0, 0 && 1, "{return 0 && 1;}");
  assert(0, 1 && 0, "{return 1 && 0;}");
  assert(1, 1 && 1, "{return 1 && 1;}");
  assert(0, 0 || 0, "{return 0 || 0;}");
  assert(1, 0 || 1, "{return 0 || 1;}");
  assert(1, 1 || 0, "{return 1 || 0;}");
  assert(1, 1 || 1, "{return 1 || 1;}");
  assert(1, !(1 && 2) || !5 && (3 && 0 == 2) || 8 >= 2, "{return !(1 && 2) || !5 && (3 && 0 == 2) || 8 >= 2;}");

  printf("All arithmetical test cases have passed.\n\n");
  return 0;
}
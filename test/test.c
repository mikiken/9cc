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
  printf("OK\n");

  return 0;
}
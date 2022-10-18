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

int main() {
  assert(0, 0, "0");
  assert(42, 42, "42");

  assert(21, 5 + 20 - 4, "5 + 20 - 4");
  assert(41, 12 + 34 - 5, "12 + 34 - 5");
  assert(148, 100 + 56 - 8, "100 + 56 - 8");

  assert(47, 5 + 6 * 7, "5+6*7");
  assert(15, 5 * (9 - 6), "5*(9-6)");
  assert(4, (3 + 5) / 2, "(3+5)/2");

  assert(10, -10 + 20, "-10+20");
  assert(10, +(+10), "+(+10)");
  assert(10, -(-10), "-(-10)");
  assert(10, (-(-15 + 25) + 50) / 4, "(-(-15 + 25) + 50) / 4");

  assert(0, 0 == 1, "0==1");
  assert(1, 42 == 42, "42==42");
  assert(1, 0 != 1, "0!=1");
  assert(0, 42 != 42, "42!=42");

  assert(1, 0 < 1, "0<1");
  assert(0, 1 < 1, "1<1");
  assert(0, 2 < 1, "2<1");
  assert(1, 0 <= 1, "0<=1");
  assert(1, 1 <= 1, "1<=1");
  assert(0, 2 <= 1, "2<=1");

  assert(1, 1 > 0, "1>0");
  assert(0, 1 > 1, "1>1");
  assert(0, 1 > 2, "1>2");
  assert(1, 1 >= 0, "1>=0");
  assert(1, 1 >= 1, "1>=1");
  assert(0, 1 >= 2, "1>=2");

  printf("OK\n");
  return 0;
}
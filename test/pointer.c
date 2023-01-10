int printf();

int assert();

int pointer_test1() {
  int x;
  int *y;
  x = 3;
  y = &x;
  return *y + 2;
}

int pointer_test2() {
  int x;
  x = 3;
  return *&x;
}

int pointer_test3() {
  int x;
  int *y;
  int **z;
  x = 3;
  y = &x;
  z = &y;
  return **z;
}

int pointer_test4() {
  int x;
  int *y;
  x = 3;
  y = &x;
  *y = 5;
  return x;
}

int pointer_plus2(int *x) {
  return *x + 2;
}

int pointer_test5() {
  int x;
  x = 3;
  return pointer_plus2(&x);
}

int no_interaction_to_ptr(int *p) {
  return 0;
}

int no_interaction_to_ptr_to_ptr(int **p) {
  return 0;
}

int alloc4_array[5];

int alloc4(int **p, int a, int b, int c, int d) {
  *p = alloc4_array;
  (*p)[0] = a;
  (*p)[1] = b;
  (*p)[2] = c;
  (*p)[3] = d;
  (*p)[4] = 100;
  return 0;
}

int pointer_test6() {
  int p;
  return no_interaction_to_ptr(&p);
}

int pointer_test7() {
  int *p;
  return no_interaction_to_ptr_to_ptr(&p);
}

int pointer_test8() {
  int a;
  int *b;
  int **c;
  int ***d;
  a = 6;
  b = &a;
  c = &b;
  d = &c;
  return ***d;
}

int pointer_test9() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  int *q;
  q = p + 2;
  return *q;
}

int pointer_test10() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  int *q;
  q = p + 2;
  q = q - 1;
  return *q;
}

int pointer_test11() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 2);
}

int pointer_test12() {
  int a[2];
  int k;
  k = &a[1] - &a[0];
  return k;
}

int pointer_test13() {
  int a[2];
  int *p;
  int *q;
  p = &a[0];
  q = &a[1];
  return q - p;
}

void pointer_test() {
  // ポインタ型
  assert(5, pointer_test1(), "{int x; int *y; x=3; y=&x; return *y+2;}");
  assert(3, pointer_test2(), "{int x; x=3; return *&x; }");
  assert(3, pointer_test3(), "{int x; int *y; int **z; x=3; y=&x; z=&y; return **z; }");
  assert(5, pointer_test4(), "{int x; int *y; x=3; y=&x; *y=5; return x; }");
  assert(5, pointer_test5(), "{int x; x = 3; return pointer_plus2(&x);}");

  // ポインタの加減算
  assert(0, pointer_test6(), "{int p; no_interaction_to_ptr(&p); return 0;}");
  assert(0, pointer_test7(), "{int *p; no_interaction_to_ptr_to_ptr(&p); return 0;}");
  assert(6, pointer_test8(), "{int a; int *b; int **c; int ***d; a=6; b=&a; c=&b; d=&c; return ***d;}");
  assert(4, pointer_test9(), "{int *p; alloc4(&p, 1, 2, 4, 8); int *q; q=p+2; return *q;}");
  assert(2, pointer_test10(), "{int *p; alloc4(&p, 1, 2, 4, 8); int *q; q=p+2; q=q-1; return *q;}");
  assert(4, pointer_test11(), "{int *p; alloc4(&p, 1, 2, 4, 8); return *(p+2);}");
  assert(1, pointer_test12(), "{int a[2]; int k; k = &a[1] - &a[0]; return k;}");
  assert(1, pointer_test13(), "{int a[2]; int *p; int *q; p = &a[0]; q = &a[1]; return q - p;}");

  printf("\x1b[32m"); // 文字色を緑に設定
  printf("All pointer test cases have passed.\n\n");
  printf("\x1b[0m");
}
int printf();
int puts();

int assert();

char char_type_test1() {
  char x[3];
  x[0] = -1;
  x[1] = 2;
  int y;
  y = 4;
  return x[0] + y;
}

char char_type_test2() {
  char x;
  x = 1;
  return x;
}

char char_type_test3() {
  char x;
  x = 1;
  char y;
  y = 2;
  return x;
}

char char_type_test4() {
  char x;
  x = 1;
  char y;
  y = 2;
  return y;
}

char char_type_test5() {
  char x;
  int y;
  y = 3;
  x = y;
  return x;
}

char char_type_test6() {
  char x;
  return sizeof(x);
}

char char_type_test7() {
  char x[10];
  return sizeof(x);
}

char sub_char(char a, char b, char c) {
  return a - b - c;
}

char char_type_test8() {
  return sub_char(7, 3, 3);
}

void string_literal_test1() {
  char *str;
  str = "Hello, implementation-defined behavior!";
  puts(str);
}

void string_literal_test2() {
  char *str;
  str = "Hello, implementation-defined behavior!\n";
  puts(str);
  return;
}

/*
int string_literal_test3() {
  char str[] = "abc";
  return str[2];
}

int string_literal_test4() {
  char str[] = "abc";
  return str[3];
}

int string_literal_test5() {
  char str[] = "";
  return str[0];
}
*/

int string_literal_test6() {
  char str[4] = "";
  return str[2];
}

int string_literal_test7() {
  char str[6] = "abc";
  return str[3];
}

void string_test() {
  // char型
  assert(3, char_type_test1(), "{char x[3]; x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y;}");
  assert(1, char_type_test2(), "{ char x; x=1; return x; }");
  assert(1, char_type_test3(), "{ char x; x=1; char y; y=2; return x; }");
  assert(2, char_type_test4(), "{ char x; x=1; char y; y=2; return y; }");
  assert(3, char_type_test5(), "{char x; int y; y = 3; x = y; return x;}");
  assert(1, char_type_test6(), "{ char x; return sizeof(x); }");
  assert(10, char_type_test7(), "{ char x[10]; return sizeof(x); }");
  assert(1, char_type_test8(), "sub_char(7, 3, 3)");

  // 文字列リテラル
  assert(97, "abc"[0], "\"abc\"[0]");
  assert(98, "abc"[1], "\"abc\"[1]");
  assert(99, "abc"[2], "\"abc\"[2]");
  assert(0, "abc"[3], "\"abc\"[3]");
  assert(0, ""[0], "\"\"[0]");
  assert(4, sizeof("abc"), "sizeof(\"abc\")");
  assert(1, sizeof(""), "sizeof(\"\")");
  string_literal_test1();
  // assert(99, string_literal_test3(), "{char str[] = \"abc\"; return str[2];}");
  // assert(0, string_literal_test4(), "{char str[] = \"abc\"; return str[3];}");
  // assert(0, string_literal_test5(), "{char str[] = \"\"; return str[0];}");
  assert(0, string_literal_test6(), "{char str[4] = \"\"; return str[2];}");
  assert(0, string_literal_test7(), "{char str[6] = \"abc\";  return str[3];}");

  // エスケープシーケンス
  assert(7, "\a"[0], "{return \"\\a\"[0];}");
  assert(8, "\b"[0], "{return \"\\b\"[0];}");
  assert(9, "\t"[0], "{return \"\\t\"[0];}");
  assert(10, "\n"[0], "{return \"\\n\"[0];}");
  assert(11, "\v"[0], "{return \"\\v\"[0];}");
  assert(12, "\f"[0], "{return \"\\f\"[0];}");
  assert(13, "\r"[0], "{return \"\\r\"[0];}");
  assert(63, "\?"[0], "{return \"\\?\"[0];}");
  assert(39, "\'"[0], "{return \"\\\'\"[0];}");
  assert(92, "\\"[0], "{return \"\\\\\"[0];}");
  assert(27, "\e"[0], "{return \"\\e\"[0];}"); // GNU拡張
  assert(34, "\""[0], "{return \"\\\"\"[0];}");
  string_literal_test2();

  // 文字リテラル
  assert(97, 'a', "'a'");
  assert(6, '6' - '0', "'6' - '0'");
  assert(7, '\a', "'\\a'");
  assert(8, '\b', "'\\b'");
  assert(9, '\t', "'\\t'");
  assert(10, '\n', "'\\n'");
  assert(11, '\v', "'\\v'");
  assert(12, '\f', "'\\f'");
  assert(13, '\r', "'\\r'");
  assert(27, '\e', "'\\e'");
  assert(39, '\'', "'\\\''");
  assert(92, '\\', "'\\\\");

  printf("\x1b[32m"); // 文字色を緑に設定
  printf("All string test cases have passed.\n\n");
  printf("\x1b[0m");
}

int printf();
int puts();
int exit();

int arith_test();
int variable_test();
int control_statement_test();
int function_test();
int pointer_test();
int array_test();
int string_test();

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

int main() {
  // 四則演算/sizeofのテスト
  arith_test();
  // ローカル変数/グローバル変数のテスト
  variable_test();
  // 制御構文のテスト
  control_statement_test();
  // 関数のテスト
  function_test();
  // ポインタのテスト
  pointer_test();
  // 配列のテスト
  array_test();
  // char型/文字列リテラル/エスケープシーケンスのテスト
  string_test();

  printf("All test cases have passed.\n");
  return 0;
}
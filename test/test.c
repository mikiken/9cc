int printf();
int puts();
int exit();

void arith_test();
void variable_test();
void control_statement_test();
void function_test();
void pointer_test();
void array_test();
void string_test();

int assert(int expected, int actual, char *code) {
  if (expected == actual) {
    printf("\x1b[32m"); // 文字色を緑に設定
    printf("[PASS] ");
    printf("\x1b[0m");
    printf("%s => %d\n", code, actual);
  }
  else {
    printf("\x1b[31m"); // 文字色を赤に設定
    printf("[FAIL] ");
    printf("\x1b[0m");
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

  printf("\x1b[32m"); // 文字色を緑に設定
  printf("All test cases have passed.\n");
  printf("\x1b[0m");
  return 0;
}
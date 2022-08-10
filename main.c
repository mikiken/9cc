#include "9cc.h"
// 入力プログラムを定義(宣言は9cc.h)
char *user_input;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("%s: 引数の個数が正しくありません", argv[0]);
    return 1;
  }

  user_input = argv[1];

  token = tokenize();
  parse();
  //Function f_head_typed = make_typed_ast(); // 型情報を付与する
  make_typed_ast();
  codegen(func_head);

  return 0;
}

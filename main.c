#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    error("%s: 引数の個数が正しくありません", argv[0]);
    return 1;
  }

  user_input = argv[1];

  Token *tok = tokenize(user_input);
  Function *func_list = parse(tok);
  add_type_to_ast(func_list);
  codegen(func_list);

  return 0;
}

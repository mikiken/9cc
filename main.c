#include "9cc.h"
// 入力プログラムを定義(宣言は9cc.h)
char *user_input;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("%s: 引数の個数が正しくありません", argv[0]);
    return 1;
  }

  label = 0;
  
  user_input = argv[1];
  // トークナイズする
  token = tokenize();
  
  parse();
  
  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  gen_prologue();

  // 先頭の式から順にコード生成
  for (Node *cur = stmt_head.next; cur; cur = cur->next) {
    gen(cur->body);
    printf("  pop rax\n");
  }

  gen_epilogue();

  return 0;
}

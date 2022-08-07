#include "9cc.h"

int label_count;
char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
Function *current_func;

#if 0
void gen_prologue() {
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  if (current_func->locals->offset)
    printf("  sub rsp, %d\n", current_func->locals->offset);
}

// 最後の式の結果がRAXに残っているのでそれが返り値になる
void gen_epilogue() {
  printf(".L.return.%s:\n", current_func->name);
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
#endif

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind) {
    case ND_STMT:
      for (Node *n = node; n; n = n->next) {
        gen(n->body);
      }
      return;
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
    case ND_RETURN:
      gen(node->lhs);
      printf("  pop rax\n"); // 返り値をraxにセット
      printf("  jmp .L.return.%s\n", current_func->name);
      return;
    case ND_IF:
    {
      int label = label_count++;
      if (node->els) {
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .L.else.%d\n",label);
        gen(node->then);
        printf("  jmp .L.end.%d\n", label);

        printf(".L.else.%d:\n", label);
        gen(node->els);

        printf(".L.end.%d:\n", label);
      } else {
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .L.end.%d\n",label);
        gen(node->then);

        printf(".L.end.%d:\n", label);
      }
      return;
    }
    case ND_FOR:
    {
      int label = label_count++;
      if (node->init)
        gen(node->init);
      printf(".L.begin.%d:\n", label);
      if (node->cond) {
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .L.end.%d\n", label);
      }
      gen(node->then);
      if (node->inc)
        gen(node->inc);
      printf("  jmp .L.begin.%d\n", label);
      printf(".L.end.%d:\n", label);
      return;
    }
    case ND_FUNCALL:
    {
      int arg_count = 0;
      for (Node *n = node->expr; n; n = n->next) {
        gen(n->body);
        arg_count++;
      }
      if (arg_count > 6)
        error("7個以上の引数をもつ関数呼び出しは現在対応していません\n");

      for (int i = arg_count - 1; i >= 0; i--)
        printf("  pop %s\n", argreg[i]);
      printf("  call %s\n", node->func_name);
      printf("  push rax\n"); // callした関数の返り値をスタックトップに積む
      return;
    }
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}

void codegen() {
  printf(".intel_syntax noprefix\n");
  // 先頭の関数から順にコード生成
  for (Function *f = func_head.next; f; f = f->next) {
    current_func = f;
    printf(".globl %s\n", f->name);
    printf("%s:\n", f->name);
    // prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    if (current_func->locals->offset)
      printf("  sub rsp, %d\n", current_func->locals->offset);
    int i = 0;
    for (Lvar *param = f->params_head.next; param; param = param->next) {
      printf("  mov rax, rbp\n");
      printf("  sub rax, %d\n", param->offset);
      printf("  mov [rax], %s\n", argreg[i++]);
    }
    // body
    gen(f->body);
    // epilogue
    printf(".L.return.%s:\n", f->name);
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
  }
}
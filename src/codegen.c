#include "9cc.h"

int label_count;

#define RAX 0
#define RDI 1
#define RSI 2
#define RDX 3
#define RCX 4
#define RBP 5
#define RSP 6
#define RBX 7
#define R8 8
#define R9 9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15

char *reg[16][4] = {
    {  "al",   "ax",  "eax", "rax"},
    { "dil",   "di",  "edi", "rdi"},
    { "sil",   "si",  "esi", "rsi"},
    {  "dl",   "dx",  "edx", "rdx"},
    {  "cl",   "cx",  "ecx", "rcx"},
    { "bpl",   "bp",  "ebp", "rbp"},
    { "spl",   "sp",  "esp", "rsp"},
    {  "bl",   "bx",  "ebx", "rbx"},
    { "r8b",  "r8w",  "r8d",  "r8"},
    { "r9b",  "r9w",  "r9d",  "r9"},
    {"r10b", "r10w", "r10d", "r10"},
    {"r11b", "r11w", "r11d", "r11"},
    {"r12b", "r12w", "r12d", "r12"},
    {"r13b", "r13w", "r13d", "r13"},
    {"r14b", "r14w", "r14d", "r14"},
    {"r15b", "r15w", "r15d", "r15"},
};

int reg_size(Type *type) {
  switch (type->kind) {
    case TYPE_INT:
      return SIZE_INT;
    case TYPE_CHAR:
      return SIZE_CHAR;
    case TYPE_PTR:
      return SIZE_PTR;
    default:
      error("未対応のデータサイズです");
  }
}

char *reg_name(int reg_kind, int size) {
  switch (size) {
    case 1:
      return reg[reg_kind][0];
    case 2:
      return reg[reg_kind][1];
    case 4:
      return reg[reg_kind][2];
    case 8:
      return reg[reg_kind][3];
    default:
      error("非対応のデータサイズです");
  }
}

int arg_reg_kind(int arg_count) {
  switch (arg_count) {
    case 1:
      return RDI;
    case 2:
      return RSI;
    case 3:
      return RDX;
    case 4:
      return RCX;
    case 5:
      return R8;
    case 6:
      return R9;
    default:
      error("引数7個以上の関数呼び出しには現在対応していません");
  }
}

void push(Type *type, int src_reg) {
  printf("  push %s\n", reg_name(src_reg, 8));
}

void pop(Type *type, int dest_reg) {
  printf("  pop %s\n", reg_name(dest_reg, 8));
}

void push_addr(int dest_reg) {
  printf("  push %s\n", reg_name(dest_reg, 8)); // アドレスだから8byte
}

void pop_addr(int dest_reg) {
  printf("  pop %s\n", reg_name(dest_reg, 8)); // アドレスだから8byte
}

void push_immediate_value(int val) {
  printf("  push %d\n", val);
}

void mov_memory_to_register(Type *type, int dest_reg, int src_reg) {
  switch (type->kind) {
    case TYPE_INT:
      printf("  mov %s, DWORD PTR [%s]\n", reg_name(dest_reg, SIZE_INT), reg_name(src_reg, 8));
      return;
    case TYPE_CHAR:
      printf("  movsx %s, BYTE PTR [%s]\n", reg_name(dest_reg, 4), reg_name(src_reg, 8));
      return;
    case TYPE_PTR:
      printf("  mov %s, [%s]\n", reg_name(dest_reg, SIZE_PTR), reg_name(src_reg, 8));
      return;
    default:
      error("レジスタに値をセットできませんでした");
      return;
  }
}

void mov_register_to_memory(Type *type, int dest_reg, int src_reg) {
  switch (type->kind) {
    case TYPE_INT:
      printf("  mov DWORD PTR [%s], %s\n", reg_name(dest_reg, 8), reg_name(src_reg, SIZE_INT));
      return;
    case TYPE_CHAR:
      printf("  mov [%s], %s\n", reg_name(dest_reg, 8), reg_name(src_reg, SIZE_CHAR));
      return;
    case TYPE_PTR:
      printf("  mov [%s], %s\n", reg_name(dest_reg, 8), reg_name(src_reg, SIZE_PTR));
      return;
    default:
      error("メモリに値をセットできませんでした");
      return;
  }
}

void pass_argument_to_register(Obj *arg, int arg_count) {
  printf("  lea rax, [rbp-%d]\n", arg->offset); // アドレスは8バイト
  mov_register_to_memory(arg->type, RAX, arg_reg_kind(arg_count));
}

int calc_gvar_size(Type *type) {
  switch (type->kind) {
    case TYPE_INT:
      return SIZE_INT;
    case TYPE_CHAR:
      return SIZE_CHAR;
    case TYPE_PTR:
      return SIZE_PTR;
    case TYPE_ARRAY:
      return calc_gvar_size(type->ptr_to) * type->array_size;
    default:
      error("不正な型のグローバル変数のサイズを計算することはできません");
  }
}

void sign_extension(int size) {
  switch (size) {
    case 2:
      printf("  cwd\n");
      return;
    case 4:
      printf("  cdq\n");
      return;
    case 8:
      printf("  cqo\n");
      return;
    default:
      error("sign_extension: 非対応のデータサイズです");
  }
}

void print_string_literal(int str_id, char *str) {
  printf(".LC%d:\n", str_id);
  printf("  .string \"");
  for (char *p = str; *p != '\0'; p++) {
    // '\a'や'\e'はgasの文法上、直接書けない('\'が無いものとしてエンコードされる)ので、
    // 別途8進コードポイントにエンコードする
    if (*p == '\\' && *(p + 1) == 'a') {
      printf("\\%03o", '\a');
      p++;
      continue;
    }
    if (*p == '\\' && *(p + 1) == 'e') {
      printf("\\%03o", '\e');
      p++;
      continue;
    }
    // それ以外のエスケープシーケンスの場合はgasがエンコードしてくれるので、
    //そのまま'\' + '<文字>'として出力する
    if (*p == '\\')
      putchar(*p++);

    putchar(*p);
  }
  printf("\"\n");
}

void gen_data_secton(Obj *gvar_list) {
  printf(".data\n");
  for (Obj *obj = gvar_list; obj->next != NULL; obj = obj->next) {
    // 文字列リテラルの場合
    if (obj->init_data)
      print_string_literal(obj->str_id, obj->init_data);
    // グローバル変数の場合
    else {
      printf("%s:\n", obj->name);
      printf("  .zero %d\n", calc_gvar_size(obj->type));
    }
  }
  printf("\n");
}

void gen_prologue(Function *func) {
  push_addr(RBP);             // 呼び出し前の関数のベースポインタをスタックにpushしておく
  printf("  mov rbp, rsp\n"); // ベースポインタをスタックトップに移動
  // ローカル変数のスタック領域を確保する
  int offset = func->lvar_list->offset;
  if (offset) {
    offset = (offset / 16 + 1) * 16;
    printf("  sub rsp, %d\n", offset);
  }
}

void gen_epilogue(Function *func) {
  printf(".L.return.%s:\n", func->name);
  printf("  mov rsp, rbp\n");
  pop_addr(RBP);
  printf("  ret\n");
}

void gen_addr();
void gen_stmt();
void gen_expr();
void gen_binary_operator_expr();

void gen_addr(Node *node) {
  switch (node->kind) {
    case ND_LVAR:
      printf("  lea rdi, [rbp-%d]\n", node->offset); // アドレスは8byte
      push_addr(RDI);                                // ローカル変数のアドレスをスタックにpush
      return;
    case ND_GVAR:
      printf("  lea rdi, [rip+%s]\n", node->gvar_name); // アドレスは8byte
      push_addr(RDI);                                   //グローバル変数のアドレスをスタックにpush
      return;
    case ND_STR:
      printf("  lea rdi, [rip+.LC%d]\n", node->str_id);
      push_addr(RDI);
      return;
    // derefが連続しているときは、更にlhsのアドレスを参照する
    case ND_DEREF:
      gen_expr(node->lhs);
      return;
    default:
      error("nodeのアドレスを生成することができません");
      return;
  }
}

void gen_stmt(Function *func, Node *node) {
  switch (node->kind) {
    case ND_STMT:
      for (Node *n = node; n; n = n->next) {
        gen_stmt(func, n->body);
      }
      return;
    case ND_RETURN:
      if (node->lhs) {
        gen_expr(node->lhs);
        pop(node->lhs->type, RAX);
      }
      printf("  jmp .L.return.%s\n", func->name);
      return;
    case ND_IF: {
      int label = label_count++;
      if (node->els) {
        gen_expr(node->cond);
        pop(node->cond->type, RAX);
        printf("  cmp %s, 0\n", reg_name(RAX, reg_size(node->cond->type)));
        printf("  je  .L.else.%d\n", label);
        gen_stmt(func, node->then);
        printf("  jmp .L.end.%d\n", label);
        printf(".L.else.%d:\n", label);
        gen_stmt(func, node->els);
        printf(".L.end.%d:\n", label);
      }
      else {
        gen_expr(node->cond);
        pop(node->cond->type, RAX);
        printf("  cmp %s, 0\n", reg_name(RAX, reg_size(node->cond->type)));
        printf("  je  .L.end.%d\n", label);
        gen_stmt(func, node->then);
        printf(".L.end.%d:\n", label);
      }
      return;
    }
    case ND_FOR: {
      int label = label_count++;
      if (node->init) {
        gen_expr(node->init);
        pop(node->init->type, RAX);
      }
      printf(".L.begin.%d:\n", label);
      if (node->cond) {
        gen_expr(node->cond);
        pop(node->cond->type, RAX);
        printf("  cmp %s, 0\n", reg_name(RAX, reg_size(node->cond->type)));
        printf("  je  .L.end.%d\n", label);
      }
      gen_stmt(func, node->then);
      if (node->inc) {
        gen_expr(node->inc);
        pop(node->inc->type, RAX);
      }
      printf("  jmp .L.begin.%d\n", label);
      printf(".L.end.%d:\n", label);
      return;
    }
    // それ以外の場合はexprのはず
    default:
      gen_expr(node);
      if (node->kind != ND_LVARDEF && node->kind != ND_ADDR && node->kind != ND_COND && node->kind != ND_COMMA)
        pop(node->type, RAX);
      return;
  }
}

void gen_expr(Node *node) {
  switch (node->kind) {
    case ND_NUM:
      push_immediate_value(node->val);
      return;
    case ND_LVARDEF:
      return;
    case ND_LVAR:
    case ND_GVAR:
      gen_addr(node);                               // 変数のアドレスをスタックにpush
      pop_addr(RDI);                                // rdiに変数のアドレスをセット
      mov_memory_to_register(node->type, RAX, RDI); // rdiの参照先の値をraxに代入する
      push(node->type, RAX);                        // raxの値をスタックにpush
      return;
    case ND_ASSIGN:
      gen_addr(node->lhs);                          // 左辺のアドレスをスタックにpush
      gen_expr(node->rhs);                          // 右辺の値を生成
      pop(node->type, RAX);                         // 右辺の値をraxにpop
      pop_addr(RDI);                                // 左辺のアドレスをrdiにpop
      mov_register_to_memory(node->type, RDI, RAX); // rdiの参照先にraxの値を代入する
      push(node->type, RAX);
      return;
    case ND_DEREF:
      gen_expr(node->lhs);
      pop_addr(RDI);
      mov_memory_to_register(node->type, RAX, RDI);
      push(node->type, RAX);
      return;
    case ND_ADDR:
      gen_addr(node->lhs); // lhsのアドレスを生成
      return;
    case ND_NOT:
      gen_expr(node->lhs);
      pop(node->lhs->type, RAX);
      printf("  cmp %s, 0\n", reg_name(RAX, reg_size(node->lhs->type)));
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      push(new_type(TYPE_INT), RAX);
      return;
    case ND_COND: {
      int label = label_count++;
      gen_expr(node->cond);
      pop(node->cond->type, RAX);
      printf("  cmp %s, 0\n", reg_name(RAX, reg_size(node->cond->type)));
      printf("  je  .L.else.%d\n", label);
      gen_expr(node->then);
      printf("  jmp .L.end.%d\n", label);
      printf(".L.else.%d:\n", label);
      gen_expr(node->els);
      printf(".L.end.%d:\n", label);
      return;
    }
    case ND_COMMA:
      gen_expr(node->lhs);
      pop(node->lhs->type, RAX); // スタックにpushされた左辺値は不要なので捨てる (こうしないと引数に直接カンマ演算子を書いたときにバグる)
      gen_expr(node->rhs);
      return;
    case ND_FUNCALL: {
      int arg_count = 0;
      Type arg_type[6]; // 現状引数6つまでの関数呼び出しにのみ対応しているため要素数は6でよい
      for (Node *n = node->expr; n; n = n->next) {
        gen_expr(n->body);
        arg_type[arg_count++] = *n->body->type;
      }
      if (arg_count > 6)
        error("7個以上の引数をもつ関数呼び出しは現在対応していません\n");

      for (int i = arg_count; i > 0; i--)
        pop(&arg_type[i - 1], arg_reg_kind(i));
      printf("  mov al, 0\n"); // printfは可変長引数を取るので、関数呼び出し前に浮動小数点数の引数の個数をalにセットする必要がある
      printf("  call %s\n", node->func_name);
      push(node->type, RAX); // callした関数の返り値をスタックトップに積む
      return;
    }
    // それ以外の場合は二項演算子のはず
    default:
      gen_binary_operator_expr(node);
      push(node->type, RAX);
  }
}

void gen_binary_operator_expr(Node *node) {
  // 二項演算子の左辺と右辺の式を生成
  gen_expr(node->lhs);
  gen_expr(node->rhs);
  // 二項演算子の両辺の式の値をレジスタにセット
  pop(node->rhs->type, RBX);
  pop(node->lhs->type, RAX);

  switch (node->kind) {
    case ND_ADD:
      printf("  add %s, %s\n", reg_name(RAX, reg_size(node->type)), reg_name(RBX, reg_size(node->type)));
      return;
    case ND_SUB:
      printf("  sub %s, %s\n", reg_name(RAX, reg_size(node->type)), reg_name(RBX, reg_size(node->type)));
      return;
    case ND_MUL:
      printf("  imul %s, %s\n", reg_name(RAX, reg_size(node->type)), reg_name(RBX, reg_size(node->type)));
      return;
    case ND_DIV:
      sign_extension(reg_size(node->type));
      printf("  idiv %s\n", reg_name(RBX, reg_size(node->type)));
      return;
    case ND_MOD:
      sign_extension(reg_size(node->type));
      printf("  idiv %s\n", reg_name(RBX, reg_size(node->type)));
      printf("  mov rax, rdx\n");
      return;
    case ND_EQ:
      printf("  cmp %s, %s\n", reg_name(RAX, reg_size(node->type)), reg_name(RBX, reg_size(node->type)));
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      return;
    case ND_NE:
      printf("  cmp %s, %s\n", reg_name(RAX, reg_size(node->type)), reg_name(RBX, reg_size(node->type)));
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      return;
    case ND_LT:
      printf("  cmp %s, %s\n", reg_name(RAX, reg_size(node->type)), reg_name(RBX, reg_size(node->type)));
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      return;
    case ND_LE:
      printf("  cmp %s, %s\n", reg_name(RAX, reg_size(node->type)), reg_name(RBX, reg_size(node->type)));
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      return;
    case ND_AND:
      printf("  cmp %s, 0\n", reg_name(RAX, reg_size(node->lhs->type)));
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      printf("  cmp %s, 0\n", reg_name(RBX, reg_size(node->rhs->type)));
      printf("  setne bl\n");
      printf("  movzb rbx, bl\n");
      printf("  and rax, rbx\n");
      return;
    case ND_OR:
      printf("  cmp %s, 0\n", reg_name(RAX, reg_size(node->lhs->type)));
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      printf("  cmp %s, 0\n", reg_name(RBX, reg_size(node->rhs->type)));
      printf("  setne bl\n");
      printf("  movzb rbx, bl\n");
      printf("  or rax, rbx\n");
      return;
    default:
      error("コード生成を行うことができませんでした");
  }
}

void codegen(Function *typed_func_list) {
  printf(".intel_syntax noprefix\n\n");
  // グローバル変数の領域確保
  gen_data_secton(global_var_list);
  printf(".text\n");
  // 先頭の関数から順にコード生成
  for (Function *f = typed_func_list; f; f = f->next) {
    printf(".global %s\n", f->name);
    printf("%s:\n", f->name);
    gen_prologue(f);
    int arg_count = 0;
    for (Obj *param = f->params_head.next; param; param = param->next) {
      pass_argument_to_register(param, ++arg_count);
    }
    gen_stmt(f, f->body);
    gen_epilogue(f);
    printf("\n");
  }
}
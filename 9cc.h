#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
  TK_RETURN,   // return
  TK_IF,       // if
  TK_ELSE,     // else
  TK_WHILE,    // while
  TK_FOR,      // for
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *start;    // トークン文字列の開始位置
  char *end;      // トークン文字列の終了位置
  int len;        // トークンの長さ
};

typedef struct Lvar Lvar;

struct Lvar {
  Lvar *next; // 次の変数またはNULL
  char *name; // 変数の名前
  int len;    // 名前の文字数
  int offset; // RBPからのオフセット
};

// ローカル変数のリスト
Lvar *locals; // リストの先頭を指すポインタ
Lvar tail; // リストの末尾を表す

// 入力プログラム
extern char *user_input;

// 現在着目しているトークン
Token *token;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

bool at_eof();

Token *tokenize();

// 抽象構文木のノードの種類
typedef enum {
  ND_STMT,
  ND_EXPR,
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_ASSIGN, // =
  ND_NUM,    // 整数
  ND_LVAR,   // ローカル変数
  ND_RETURN, // return
  ND_IF,     // if
  ND_FOR,    // for | while
  ND_FUNCALL,// function call
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺(left-hand side)
  Node *rhs;     // 右辺(right-hand side)
  int val;       // kindがND_NUMの場合のみ使う
  int offset;    // kindがND_LVARの場合のみ使う
  // if (cond) then; els...
  Node *cond;
  Node *then;
  Node *els;
  // for (init; cond; inc) then;
  Node *init;
  Node *inc;
  // kind == ND_STMT || ND_EXPR
  Node *body;
  Node *next;
  // func_name(expr, ...)
  char *func_name;
  Node *expr;
};

typedef struct Function Function;

struct Function {
  Function *next;
  Node *body;
  char *name;
};

Function func_head; // 関数のリストの先頭

void parse();
void codegen();

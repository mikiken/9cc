#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TYPE_NULL,
  TYPE_INT, // int
  TYPE_PTR, // pointer to ...
  TYPE_ARRAY,
} TypeKind;

typedef struct Type Type;

struct Type {
  TypeKind kind; // 型の種類
  Type *ptr_to;  // kind == TYPE_PTR
  int array_size;
};

#define SIZE_INT 4
#define SIZE_PTR 8

typedef struct FuncDeclaration FuncDeclaration;

struct FuncDeclaration {
  FuncDeclaration *next;
  Type *ret_type;  // return type
  char *name;  // 関数名
  int len;     // 関数名の長さ
};

FuncDeclaration *func_declaration_list; // 宣言のリスト
FuncDeclaration func_declaration_tail;

// トークンの種類
typedef enum {
  TK_EQUAL,             // ==
  TK_NOT_EQUAL,         // !=
  TK_LESS_EQUAL,        // <=
  TK_GREATER_EQUAL,     // >=
  TK_LESS,              // <
  TK_GREATER,           // >
  TK_PLUS,              // +
  TK_MINUS,             // -
  TK_ASTERISK,          // *
  TK_SLASH,             // /
  TK_ASSIGN,            // =
  TK_SEMICOLON,         // ;
  TK_COMMA,             // ,
  TK_AND,               // &
  TK_LEFT_PARENTHESIS,  // (
  TK_RIGHT_PARENTHESIS, // )
  TK_LEFT_BRACE,        // {
  TK_RIGHT_BRACE,       // }
  TK_LEFT_BRACKET,      // [
  TK_RIGHT_BRACKET,     // ]
  TK_IDENT,             // 識別子
  TK_NUM,               // 整数トークン
  TK_EOF,               // 入力の終わりを表すトークン
  TK_RETURN,            // return
  TK_IF,                // if
  TK_ELSE,              // else
  TK_WHILE,             // while
  TK_FOR,               // for
  TK_INT,               // int 
  TK_SIZEOF,            // sizeof
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

// 入力プログラム
char *user_input;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void unexpected_symbol_error(char *loc, TokenKind kind);

Token *tokenize();

// 抽象構文木のノードの種類
typedef enum {
  ND_STMT,
  ND_EXPR,
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_EQ,      // ==
  ND_NE,      // !=
  ND_LT,      // <
  ND_LE,      // <=
  ND_ASSIGN,  // =
  ND_NUM,     // 整数
  ND_LVAR,    // ローカル変数
  ND_RETURN,  // return
  ND_IF,      // if
  ND_FOR,     // for | while
  ND_FUNCALL, // function call
  ND_ADDR,    // &
  ND_DEREF,   // *(参照)
  ND_SIZEOF,  // sizeof 意味解析でND_NUMに置き換えられる
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺(left-hand side)
  Node *rhs;     // 右辺(right-hand side)
  int val;       // kindがND_NUMの場合のみ使う
  int offset;    // kindがND_LVARの場合のみ使う
  Type *type;
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
  // kind == ND_FUNCALL  func_name(expr, ...)
  char *func_name;
  Node *expr;
};

typedef struct Lvar Lvar;

struct Lvar {
  Lvar *next; // 次の変数またはNULL
  Type *type; // 型
  char *name; // 変数の名前
  int len;    // 名前の文字数
  int offset; // RBPからのオフセット
};

typedef struct Function Function;

struct Function {
  Function *next;
  Type *type;       // 型
  char *name;       // 関数名
  Lvar params_head; // 引数リストの先頭
  Node *body;       // statement
  Lvar *locals;     // ローカル変数のリスト
};

Function func_head; // 関数のリストの先頭

void parse();
Node *new_node(NodeKind kind);
void make_typed_ast();
void codegen();

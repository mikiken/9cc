#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_INT 4
#define SIZE_PTR 8
#define SIZE_CHAR 1

// 型の種類
typedef enum {
  TYPE_NULL,
  TYPE_INT,   // int
  TYPE_CHAR,  // char
  TYPE_PTR,   // pointer to ...
  TYPE_ARRAY, // 配列型
} TypeKind;

typedef struct Type Type;

struct Type {
  TypeKind kind;  // 型の種類
  Type *ptr_to;   // kind == TYPE_PTR || TYPE_ARRAY のとき使用
  int array_size; // 配列の要素数
};

typedef struct Obj Obj;

struct Obj {
  Obj *next;  // 次のオブジェクトまたはNULL
  Type *type; // 型
  int len;    // 識別子または文字列リテラルの文字数

  // 変数
  char *name; // 変数の名前
  int offset; // RBPからのオフセット(ローカル変数のときのみ使用)

  // 文字列リテラル
  char *init_data;
  int str_id;
};

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
  TK_PERCENT,           // %
  TK_ASSIGN,            // =
  TK_ADD_ASSIGN,        // +=
  TK_SUB_ASSIGN,        // -=
  TK_MUL_ASSIGN,        // *=
  TK_DIV_ASSIGN,        // /=
  TK_MOD_ASSIGN,        // %=
  TK_INCREMENT,         // ++
  TK_DECREMENT,         // --
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
  TK_STR,               // 文字列リテラル
  TK_EOF,               // 入力の終わりを表すトークン
  TK_RETURN,            // return
  TK_IF,                // if
  TK_ELSE,              // else
  TK_WHILE,             // while
  TK_FOR,               // for
  TK_INT,               // int
  TK_CHAR,              // char
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

// 抽象構文木のノードの種類
typedef enum {
  ND_STMT,
  ND_EXPR,
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_MOD,     // %
  ND_EQ,      // ==
  ND_NE,      // !=
  ND_LT,      // <
  ND_LE,      // <=
  ND_ASSIGN,  // =
  ND_NUM,     // 整数
  ND_STR,     // 文字列リテラル
  ND_LVARDEF, // ローカル変数の定義
  ND_LVAR,    // ローカル変数
  ND_GVAR,    // グローバル変数
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
  // kind == ND_GVAR
  char *gvar_name;
  // kind == ND_STR
  int str_id;
};

typedef struct FuncDeclaration FuncDeclaration;

struct FuncDeclaration {
  FuncDeclaration *next; // 次の関数宣言
  Type *ret_type;        // return type
  char *name;            // 関数名
  int len;               // 関数名の長さ
};

typedef struct Function Function;

struct Function {
  Function *next;
  Type *type;      // 型
  char *name;      // 関数名
  Obj params_head; // 引数リストの先頭
  Node *body;      // statement
  Obj *lvar_list;  // ローカル変数のリスト
};

// 入力プログラムの名前
char *file_name;
// 入力プログラム
char *user_input;

FuncDeclaration *func_declaration_list; // 関数宣言のリスト
FuncDeclaration func_declaration_tail;

Obj *global_var_list; // グローバル変数のリスト
Obj global_var_tail;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void unexpected_symbol_error(char *loc, TokenKind kind);

Node *new_node(NodeKind kind);
Type *new_type(TypeKind kind);
int base_type_size(Type *type);

char *read_file(char *path);
Function *parse(Token *tok);
Token *tokenize(char *user_input);
void add_type_to_ast(Function *func_list);
void codegen(Function *typed_func_list);

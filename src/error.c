#include "9cc.h"

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "\x1b[31m"); // 文字色を赤に設定
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  fprintf(stderr, "\x1b[0m");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  // locが含まれている行の開始地点と終了地点を取得
  char *line = loc;
  while (user_input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while (*end != '\n')
    end++;

  // 見つかった行が全体の何行目か調べる
  int line_num = 1;
  for (char *p = user_input; p < line; p++)
    if (*p == '\n')
      line_num++;

  // 見つかった行をファイル名と行番号と一緒に表示
  fprintf(stderr, "\x1b[31m"); // 文字色を赤に設定
  fprintf(stderr, "[Error] ");
  fprintf(stderr, "\x1b[0m");
  fprintf(stderr, "in %s : %d\n", file_name, line_num);
  int indent = fprintf(stderr, "    %d |", line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  // エラー箇所を"^"を指し示して、エラーメッセージを表示
  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
  fprintf(stderr, "\x1b[31m");      // 文字色を赤に設定
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  fprintf(stderr, "\x1b[0m");
  exit(1);
}

void unexpected_symbol_error(char *loc, TokenKind kind) {
  switch (kind) {
    case TK_EQUAL:
      error_at(loc, "'=='ではありません");
      return;
    case TK_NOT_EQUAL:
      error_at(loc, "'!='ではありません");
      return;
    case TK_LESS_EQUAL:
      error_at(loc, "'<='ではありません");
      return;
    case TK_GREATER_EQUAL:
      error_at(loc, "'>='ではありません");
      return;
    case TK_LESS:
      error_at(loc, "'<'ではありません");
      return;
    case TK_GREATER:
      error_at(loc, "'>'ではありません");
      return;
    case TK_PLUS:
      error_at(loc, "'+'ではありません");
      return;
    case TK_MINUS:
      error_at(loc, "'-'ではありません");
      return;
    case TK_ASTERISK:
      error_at(loc, "'*'ではありません");
      return;
    case TK_SLASH:
      error_at(loc, "'/'ではありません");
      return;
    case TK_ASSIGN:
      error_at(loc, "'='ではありません");
      return;
    case TK_SEMICOLON:
      error_at(loc, "';'ではありません");
      return;
    case TK_COMMA:
      error_at(loc, "','ではありません");
      return;
    case TK_AND:
      error_at(loc, "'&'ではありません");
      return;
    case TK_LEFT_PARENTHESIS:
      error_at(loc, "'('ではありません");
      return;
    case TK_RIGHT_PARENTHESIS:
      error_at(loc, "')'ではありません");
      return;
    case TK_LEFT_BRACE:
      error_at(loc, "'{'ではありません");
      return;
    case TK_RIGHT_BRACE:
      error_at(loc, "'}'ではありません");
      return;
    case TK_LEFT_BRACKET:
      error_at(loc, "'['ではありません");
      return;
    case TK_RIGHT_BRACKET:
      error_at(loc, "']'ではありません");
      return;
    case TK_IDENT:
      error_at(loc, "識別子ではありません");
      return;
    case TK_NUM:
      error_at(loc, "整数ではありません");
      return;
    case TK_EOF:
      error_at(loc, "入力の終端ではありません");
      return;
    case TK_RETURN:
      error_at(loc, "'return'ではありません");
      return;
    case TK_IF:
      error_at(loc, "'if'ではありません");
      return;
    case TK_ELSE:
      error_at(loc, "'else'ではありません");
      return;
    case TK_WHILE:
      error_at(loc, "'while'ではありません");
      return;
    case TK_FOR:
      error_at(loc, "'for'ではありません");
      return;
    case TK_INT:
      error_at(loc, "'int'ではありません");
      return;
    case TK_SIZEOF:
      error_at(loc, "'sizeof'ではありません");
      return;
    default:
      error_at(loc, "予期しないトークンの種類です");
      return;
  }
}
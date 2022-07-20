#include "9cc.h"
// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input; // エラー発生箇所が何文字目か計算
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 二項演算子が指定したものであるかチェック
bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

bool is_ident_first(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool is_ident(char c) {
  return is_ident_first(c) || ('0' <= c && c <= '9'); 
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *start, char*end) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->start = start;
  tok->end = end;
  tok->len = end - start + 1;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, p+1);
      p += 2;
      continue;
    }

    if (strchr("<>+-*/()=;", *p)) {
      cur = new_token(TK_RESERVED, cur, p, p);
      p++;
      continue;
    }

    if (strncmp(p, "if", 2) == 0 && !is_ident(*(p+2))) {
      cur = new_token(TK_IF, cur, p, p+1);
      p += 2;
      continue;
    }

    #if 0
      if (strncmp(p, "else", 4) == 0 && !is_ident(*(p+4))) {
        cur = new_token(TK_ELSE, cur, p, p+3);
        p += 4;
        continue;
      }
    #endif

    if (strncmp(p, "return", 6) == 0 && !is_ident(*(p+6))) {
      cur = new_token(TK_RETURN, cur, p, p+5);
      p += 6;
      continue;
    }

    if (is_ident_first(*p)) {
      char *start = p;
      do {
        p++;
      } while (is_ident(*p));
      cur = new_token(TK_IDENT, cur, start, --p);
      p++;
      continue;
    }

    if (isdigit(*p)) {
      char *start = p; // 読み込み開始位置を記録
      int value = strtol(p, &p, 10);
      cur = new_token(TK_NUM, cur, start, --p);
      p++;
      cur->val = value;
      continue;
    }

    error_at(p, "解釈できない文字です");
  }

  cur = new_token(TK_EOF, cur, p, p);
  cur->len = 0;

  return head.next; // headはダミーノードなので、その次のノードを返す
}

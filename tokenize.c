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

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token)); // calloc: メモリ領域を確保し、0埋め (要素数, 1要素あたりのサイズ)
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
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
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (strchr("<>+-*/()=;", *p)) {
      cur = new_token(TK_RESERVED, cur, p, 1);
      p++;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p, 1);
      p++;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0); // この段階では数字が何文字か分からにので0としておく
      char *q = p; // 読み込み開始位置を記録
      cur->val = strtol(p, &p, 10);
      cur->len = p - q; // 読み込み前後の位置の差から、文字数を計算
      continue;
    }

    error_at(p, "解釈できない文字です");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next; // headはダミーノードなので、その次のノードを返す
}

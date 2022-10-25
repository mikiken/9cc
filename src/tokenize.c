#include "9cc.h"

// 記号が指定したものであるか判定
bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

//　文字列の最初の文字が識別子として適合するか判定
bool is_ident_first(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// 文字列の途中の文字が識別子として適合するか判定
bool is_ident(char c) {
  return is_ident_first(c) || ('0' <= c && c <= '9');
}

// 文字列が引数で指定したキーワードであるか判定
bool is_keyword(char *p, char *keyword) {
  return strncmp(p, keyword, strlen(keyword)) == 0 && !is_ident(*(p + strlen(keyword)));
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *start, char *end) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->start = start;
  tok->end = end;
  tok->len = end - start + 1;
  cur->next = tok;
  return tok;
}

char read_escape_char(char *p) {
  switch (*p) {
    case 'a':
      return 7;
    case 'e':
      return 27;
    default:
      return *p;
  }
}

char *find_string_literal_end(char *start) {
  char *p;
  for (p = start; *p != '\"'; p++) {
    if (*p == '\0' || *p == '\n')
      error_at(start, "文字列リテラルが閉じていません");
    if (*p == '\\')
      p++;
  }
  return p;
}

// 入力文字列をトークナイズしてそれを返す
Token *tokenize(char *user_input) {
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

    // 行コメントをスキップ
    if (startswith(p, "//")) {
      p += 2;
      while (*p != '\n')
        p++;
      continue;
    }

    // ブロックコメントをスキップ
    if (startswith(p, "/*")) {
      char *end = strstr(p + 2, "*/");
      if (!end)
        error_at(p, "ブロックコメントが閉じていません");
      p = end + 2;
      continue;
    }

    if (startswith(p, "==")) {
      cur = new_token(TK_EQUAL, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, "!=")) {
      cur = new_token(TK_NOT_EQUAL, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, "<=")) {
      cur = new_token(TK_LESS_EQUAL, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, ">=")) {
      cur = new_token(TK_GREATER_EQUAL, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, "<")) {
      cur = new_token(TK_LESS, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, ">")) {
      cur = new_token(TK_GREATER, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "+=")) {
      cur = new_token(TK_ADD_ASSIGN, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, "-=")) {
      cur = new_token(TK_SUB_ASSIGN, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, "*=")) {
      cur = new_token(TK_MUL_ASSIGN, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, "/=")) {
      cur = new_token(TK_DIV_ASSIGN, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, "%=")) {
      cur = new_token(TK_MOD_ASSIGN, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (startswith(p, "+")) {
      cur = new_token(TK_PLUS, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "-")) {
      cur = new_token(TK_MINUS, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "*")) {
      cur = new_token(TK_ASTERISK, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "/")) {
      cur = new_token(TK_SLASH, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "%")) {
      cur = new_token(TK_PERCENT, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "=")) {
      cur = new_token(TK_ASSIGN, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, ";")) {
      cur = new_token(TK_SEMICOLON, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, ",")) {
      cur = new_token(TK_COMMA, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "&")) {
      cur = new_token(TK_AND, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "(")) {
      cur = new_token(TK_LEFT_PARENTHESIS, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, ")")) {
      cur = new_token(TK_RIGHT_PARENTHESIS, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "{")) {
      cur = new_token(TK_LEFT_BRACE, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "}")) {
      cur = new_token(TK_RIGHT_BRACE, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "[")) {
      cur = new_token(TK_LEFT_BRACKET, cur, p, p);
      p++;
      continue;
    }

    if (startswith(p, "]")) {
      cur = new_token(TK_RIGHT_BRACKET, cur, p, p);
      p++;
      continue;
    }

    if (is_keyword(p, "if")) {
      cur = new_token(TK_IF, cur, p, p + 1);
      p += 2;
      continue;
    }

    if (is_keyword(p, "else")) {
      cur = new_token(TK_ELSE, cur, p, p + 3);
      p += 4;
      continue;
    }

    if (is_keyword(p, "while")) {
      cur = new_token(TK_WHILE, cur, p, p + 4);
      p += 5;
      continue;
    }

    if (is_keyword(p, "for")) {
      cur = new_token(TK_FOR, cur, p, p + 2);
      p += 3;
      continue;
    }

    if (is_keyword(p, "return")) {
      cur = new_token(TK_RETURN, cur, p, p + 5);
      p += 6;
      continue;
    }

    if (is_keyword(p, "int")) {
      cur = new_token(TK_INT, cur, p, p + 2);
      p += 3;
      continue;
    }

    if (is_keyword(p, "char")) {
      cur = new_token(TK_CHAR, cur, p, p + 3);
      p += 4;
      continue;
    }

    if (is_keyword(p, "sizeof")) {
      cur = new_token(TK_SIZEOF, cur, p, p + 5);
      p += 6;
      continue;
    }

    // 識別子の場合
    if (is_ident_first(*p)) {
      char *start = p;
      do {
        p++;
      } while (is_ident(*p));
      cur = new_token(TK_IDENT, cur, start, p - 1);
      continue;
    }

    // 文字列リテラルの場合
    if (startswith(p, "\"")) {
      char *start = ++p; // ダブルクオートを読み飛ばす
      char *end = find_string_literal_end(p);
      char *buf = calloc(end - start, sizeof(char));
      int len = 0;
      while (*p != '\"') {
        // エスケープシーケンスの場合
        if (*p == '\\') {
          buf[len++] = '\\';
          buf[len++] = read_escape_char(p + 1);
          p += 2;
        }
        else {
          buf[len++] = *p++;
        }
      }
      cur = new_token(TK_STR, cur, buf, buf + len - 1);
      p++; // ダブルクオートを読み飛ばす
      continue;
    }

    // 整数の場合
    if (isdigit(*p)) {
      char *start = p; // 読み込み開始位置を記録
      int value = strtol(p, &p, 10);
      cur = new_token(TK_NUM, cur, start, p - 1);
      cur->val = value;
      continue;
    }

    error_at(p, "解釈できない文字です");
  }

  cur = new_token(TK_EOF, cur, p, p);
  cur->len = 0;

  return head.next; // headはダミーノードなので、その次のノードを返す
}

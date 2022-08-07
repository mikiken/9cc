#include "9cc.h"

Function *cur_func;

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(TokenKind kind, char *op) {
  // (tokenが記号でない場合) || (指定されたopの長さがtokenの長さと等しくない場合) || (指定されたopとtokenが等しくない場合)
  if (token->kind != kind || strlen(op) != token->len || memcmp(token->start, op, token->len))
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->start, op, token->len))
    error_at(token->start, "'%s'ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->start, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

Lvar *find_lvar(Token *tok) {
  for (Lvar *var = cur_func->locals; var != NULL; var = var->next)
    if (var->len == tok->len && !memcmp(tok->start, var->name, var->len))
      return var;
  return NULL;
}

Node *new_lvar_node(Token *tok) {
  Node *node = new_node(ND_LVAR);
  Lvar *lvar = find_lvar(tok);
  
  if (lvar != NULL) {
    node->offset = lvar->offset;
  } else { // 初登場のローカル変数の場合、localsの先頭に繋ぐ
    lvar = calloc(1, sizeof(Lvar));
    lvar->len = tok->len;
    lvar->offset = cur_func->locals->offset + 8;
    lvar->name = tok->start;
    lvar->next = cur_func->locals;
    cur_func->locals = lvar;
    
    node->offset = lvar->offset;
  }
  return node;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void parse() {
  //init_locals();
  program();
}

void program() {
  cur_func = &func_head;
  while (!at_eof()) {
    if (token->kind != TK_IDENT)
      error_at(token->start, "関数ではありません");
    Token *tok = token; // 関数名
    token = token->next;
    expect("(");
    // 関数定義
    {
      cur_func = cur_func->next = calloc(1, sizeof(Function));
      cur_func->name = calloc(tok->len, sizeof(char));
      memcpy(cur_func->name, tok->start, tok->len);
    }
    // 引数
    {
      cur_func->params_head.next = NULL;
      cur_func->params_head.name = NULL;
      cur_func->params_head.len = 0;
      cur_func->params_head.offset = 0;
      if (!consume(TK_RESERVED, ")")) {
        Lvar *cur_param = &cur_func->params_head;
        do {
          cur_param->next = calloc(1, sizeof(Lvar));
          cur_param->next->name = calloc(token->len, sizeof(char));
          memcpy(cur_param->next->name, token->start, token->len);
          cur_param->next->len = token->len;
          cur_param->next->offset = cur_param->offset + 8;
          cur_param = cur_param->next;
          token = token->next;
        } while (consume(TK_RESERVED, ","));
        cur_param->next = NULL;
        expect(")");
      }
    }
    expect("{");
    // ローカル変数
    Lvar lvar_tail;
    lvar_tail.len = 0;
    lvar_tail.name = NULL;
    lvar_tail.next = NULL;
    lvar_tail.offset = 0;
    cur_func->locals = &lvar_tail;
    // パラメータをローカル変数にコピー
    for ( Lvar *cur_param = cur_func->params_head.next; cur_param; cur_param = cur_param->next) {
      Lvar *new_lvar = calloc(1, sizeof(Lvar));
      new_lvar->name = calloc(cur_param->len, sizeof(char));
      memcpy(new_lvar->name, cur_param->name, cur_param->len);
      new_lvar->len = cur_param->len;
      new_lvar->offset = cur_param->offset;
      new_lvar->next = cur_func->locals;
      cur_func->locals = new_lvar;
    }
    // statement
    cur_func->body = new_node(ND_STMT);
    Node head;
    Node *cur_stmt = &head;
    while (!consume(TK_RESERVED, "}")) {
      cur_stmt = cur_stmt->next = new_node(ND_STMT);
      cur_stmt->body = stmt();
    }
    cur_func->body= head.next;
  }
  // 関数のリストの末端
  cur_func->next = NULL;
}

Node *stmt() {
  Node *node;
  if (consume(TK_IF, "if")) {
    node = new_node(ND_IF);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume(TK_ELSE, "else"))
      node->els = stmt();
  }

  else if (consume(TK_RETURN, "return")) {
    node = new_node(ND_RETURN);
    node->lhs = expr();
    expect(";");
  }

  else if (consume(TK_WHILE, "while")) {
    node = new_node(ND_FOR);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
  }

  else if (consume(TK_FOR, "for")) {
    node = new_node(ND_FOR);
    expect("(");
    if (*(token->start) != ';')
      node->init = expr();
    expect(";");
    if (*(token->start) != ';')
      node->cond = expr();
    expect(";");
    if (*(token->start) != ')')
      node->inc = expr();
    expect(")");
    node->then = stmt();
  }

  else if (consume(TK_RESERVED, "{")) {
    node = new_node(ND_STMT);
    Node head;
    Node *cur = &head;
    while (!consume(TK_RESERVED, "}")) {
      cur = cur->next = new_node(ND_STMT);
      cur->body = stmt();
    }
    node = head.next;
  }
  
  else {
    node = expr();
    expect(";");
  }
  
  return node;
}


Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume(TK_RESERVED, "="))
    node = new_binary(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  while (true) {
    if (consume(TK_RESERVED, "=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume(TK_RESERVED, "!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  while (true) {
    if (consume(TK_RESERVED, "<"))
      node = new_binary(ND_LT, node, add());
    else if (consume(TK_RESERVED, "<="))
      node = new_binary(ND_LE, node, add());
    else if (consume(TK_RESERVED, ">"))
      node = new_binary(ND_LT, add(), node);
    else if (consume(TK_RESERVED, ">="))
      node = new_binary(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  while (true) {
    if (consume(TK_RESERVED, "+"))
      node = new_binary(ND_ADD, node, mul());
    else if (consume(TK_RESERVED, "-"))
      node = new_binary(ND_SUB, node, mul());
    else
      return node; // 数値のみの場合
  }
}

Node *mul() {
  Node *node = unary();

  while (true) {
    if (consume(TK_RESERVED, "*"))
      node = new_binary(ND_MUL, node, unary());
    else if (consume(TK_RESERVED, "/"))
      node = new_binary(ND_DIV, node, unary());
    else
      return node; // 数値のみの場合
  }
}

Node *unary() {
  if (consume(TK_RESERVED, "+"))
    return new_binary(ND_ADD, new_num(0), unary());
  if (consume(TK_RESERVED, "-"))
    return new_binary(ND_SUB, new_num(0), unary());
  return primary();
}

Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume(TK_RESERVED, "(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  else if (token->kind == TK_IDENT) {
    Node *node;
    Token *tok = token;
    token = token->next;

    if (consume(TK_RESERVED, "(")) { // 関数呼び出しの場合
      node = new_node(ND_FUNCALL);
      node->func_name = calloc(tok->len, sizeof(char));
      memcpy(node->func_name, tok->start, tok->len);

      if (!consume(TK_RESERVED, ")")) {
        Node head;
        Node *cur = &head;
        do {
          cur = cur->next = new_node(ND_EXPR);
          cur->body = expr();
        } while (consume(TK_RESERVED, ","));
        expect(")");
        node->expr = head.next;
      }     
    }
    else { // ローカル変数の場合
      node = new_lvar_node(tok);
    }
    return node;
  }

  else
    return new_num(expect_number()); // それ以外は整数のはず
}

#include "9cc.h"

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

Lvar *find_lvar() {
  for (Lvar *var = locals; var != NULL; var = var->next)
    if (var->len == token->len && !memcmp(token->start, var->name, var->len))
      return var;
  return NULL;
}

void *init_locals() {
  Lvar head;
  head.offset = 0;
  head.next = NULL;
  locals = &head;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

Node *code[100];

//void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void program() {
  int i = 0;
  while (!at_eof()) {
    code[i] = stmt();
    i++;
  }
  code[i] = NULL;
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
    node = new_node(ND_WHILE);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
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
  else if (token->kind == TK_IDENT) { // ローカル変数の場合
    Node *node = new_node(ND_LVAR);
    Lvar *lvar = find_lvar();
    
    if (lvar != NULL) {
      node->offset = lvar->offset;
    } else { // 初登場のローカル変数の場合、localsの先頭に繋ぐ
      lvar = calloc(1, sizeof(Lvar));
      lvar->len = token->len;
      lvar->offset = locals->offset + 8;
      lvar->name = token->start;
      lvar->next = locals;
      locals = lvar;
      
      node->offset = lvar->offset;
    }
    token = token->next;
    return node;
  }
  else
    return new_num(expect_number()); // それ以外は整数のはず
}

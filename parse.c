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
void expect(TokenKind kind, char *op) {
  if (token->kind != kind || strlen(op) != token->len || memcmp(token->start, op, token->len))
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

Node *new_lvar_node(Type *type, Token *tok) {
  Node *node = new_node(ND_LVAR);
  Lvar *lvar = find_lvar(tok);
  
  if (lvar != NULL) {
    error_at(tok->start, "定義済みの変数を再定義することはできません");
  } else { // 初登場のローカル変数の場合、localsの先頭に繋ぐ
    lvar = calloc(1, sizeof(Lvar));
    lvar->len = tok->len;
    lvar->offset = cur_func->locals->offset + 8;
    lvar->name = tok->start;
    lvar->type = type;
    lvar->next = cur_func->locals;
    cur_func->locals = lvar;
    
    node->offset = lvar->offset;
  }
  return node;
}

Node *lvar_node(Token *tok) {
  Node *node = new_node(ND_LVAR);
  Lvar *lvar = find_lvar(tok);

  if (lvar == NULL) {
    error_at(tok->start, "未定義の変数です");
  } else {
    node->offset = lvar->offset;
  }
  return node;
}

Type *specify_type() {
  if (!consume(TK_TYPE, "int"))
    return NULL;
  Type *type = calloc(1, sizeof(Type));
  type->kind = TYPE_INT;
  while (consume(TK_RESERVED, "*")) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TYPE_PTR;
    ty->ptr_to = type;
    type = ty;
  }
  return type;
}

void init_func_declaration() {
  func_declaration_tail.ret_type = NULL;
  func_declaration_tail.name = "";
  func_declaration_tail.len = 0;
  func_declaration_list = &func_declaration_tail;
}

void add_func_declaration(Type *type, char *name, int len) {
  FuncDeclaration *new_func_declaration = calloc(1, sizeof(FuncDeclaration));
  new_func_declaration->ret_type = type;
  new_func_declaration->name = calloc(len + 1, sizeof(char));
  memcpy(new_func_declaration->name, name, len);
  new_func_declaration->len = len;
  new_func_declaration->next = func_declaration_list;
  func_declaration_list = new_func_declaration;
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
  init_func_declaration();
  program();
}

void program() {
  cur_func = &func_head;
  while (!at_eof()) {
    Type *func_type = specify_type();
    Token *func_name_token = token; // 関数名
    token = token->next;
    expect(TK_RESERVED, "(");
    if (token->start[0] == ')' && token->next->start[0] == ';') { // 関数宣言の場合
      char *func_name = calloc(func_name_token->len + 1, sizeof(char));
      // 1個少なくmemcpyすることで最後のバイトがnull terminatorになる
      memcpy(func_name, func_name_token->start, func_name_token->len);
      add_func_declaration(func_type, func_name, func_name_token->len);
      token = token->next->next;
      continue;
    }
    // 関数定義
    {
      cur_func = cur_func->next = calloc(1, sizeof(Function));
      cur_func->type = func_type;
      cur_func->name = calloc(func_name_token->len + 1, sizeof(char));
      memcpy(cur_func->name, func_name_token->start, func_name_token->len);
      // 1個少なくmemcpyすることで最後のバイトがnull terminatorになる
      add_func_declaration(func_type, cur_func->name, func_name_token->len);
    }
    // 引数
    {
      cur_func->params_head.next = NULL;
      cur_func->params_head.name = "";
      cur_func->params_head.len = 0;
      cur_func->params_head.offset = 0;
      if (!consume(TK_RESERVED, ")")) {
        Lvar *cur_param = &cur_func->params_head;
        do {
          cur_param->next = calloc(1, sizeof(Lvar));
          cur_param->next->type = specify_type(); // parameterの型
          cur_param->next->name = calloc(token->len + 1, sizeof(char));
          memcpy(cur_param->next->name, token->start, token->len);
          cur_param->next->len = token->len;
          cur_param->next->offset = cur_param->offset + 8;
          cur_param = cur_param->next;
          token = token->next;
        } while (consume(TK_RESERVED, ","));
        cur_param->next = NULL;
        expect(TK_RESERVED, ")");
      }
    }
      expect(TK_RESERVED, "{");
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
        new_lvar->name = calloc(cur_param->len + 1, sizeof(char));
        memcpy(new_lvar->name, cur_param->name, cur_param->len);
        new_lvar->len = cur_param->len;
        new_lvar->offset = cur_param->offset;
        new_lvar->type = cur_param->type;
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
    expect(TK_RESERVED, "(");
    node->cond = expr();
    expect(TK_RESERVED, ")");
    node->then = stmt();
    if (consume(TK_ELSE, "else"))
      node->els = stmt();
  }

  else if (consume(TK_RETURN, "return")) {
    node = new_node(ND_RETURN);
    node->lhs = expr();
    expect(TK_RESERVED, ";");
  }

  else if (consume(TK_WHILE, "while")) {
    node = new_node(ND_FOR);
    expect(TK_RESERVED, "(");
    node->cond = expr();
    expect(TK_RESERVED, ")");
    node->then = stmt();
  }

  else if (consume(TK_FOR, "for")) {
    node = new_node(ND_FOR);
    expect(TK_RESERVED, "(");
    if (*(token->start) != ';')
      node->init = expr();
    expect(TK_RESERVED, ";");
    if (*(token->start) != ';')
      node->cond = expr();
    expect(TK_RESERVED, ";");
    if (*(token->start) != ')')
      node->inc = expr();
    expect(TK_RESERVED, ")");
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
    expect(TK_RESERVED, ";");
  }
  
  return node;
}

Node *expr() {
  Type *ty;
  if ((ty = specify_type()) != NULL) {
    Node *node = new_lvar_node(ty, token);

    token = token->next;
    return node;
  }
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
  if (consume(TK_SIZEOF, "sizeof")) {
    Node *node = new_node(ND_SIZEOF);
    node->lhs = unary();
    return node;
  }
  if (consume(TK_RESERVED, "+"))
    return new_binary(ND_ADD, new_num(0), primary());
  if (consume(TK_RESERVED, "-"))
    return new_binary(ND_SUB, new_num(0), primary());
  if (consume(TK_RESERVED, "*")) {
    Node *node = new_node(ND_DEREF);
    node->lhs = unary();
    return node;
  }
  if (consume(TK_RESERVED, "&")) {
    Node *node = new_node(ND_ADDR);
    node->lhs = unary();
    return node;
  }
  return primary();
}

Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume(TK_RESERVED, "(")) {
    Node *node = expr();
    expect(TK_RESERVED, ")");
    return node;
  }

  else if (token->kind == TK_IDENT) {
    Node *node;
    Token *tok = token;
    token = token->next;

    if (consume(TK_RESERVED, "(")) { // 関数呼び出しの場合
      node = new_node(ND_FUNCALL);
      node->func_name = calloc(tok->len + 1, sizeof(char));
      memcpy(node->func_name, tok->start, tok->len);

      if (!consume(TK_RESERVED, ")")) {
        Node head;
        Node *cur = &head;
        do {
          cur = cur->next = new_node(ND_EXPR);
          cur->body = expr();
        } while (consume(TK_RESERVED, ","));
        expect(TK_RESERVED, ")");
        node->expr = head.next;
      }
    }
    else { // ローカル変数の場合
      node = lvar_node(tok);
    }
    return node;
  }

  else
    return new_num(expect_number()); // それ以外は整数のはず
}

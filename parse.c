#include "9cc.h"

Function *cur_func;

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// 整数ノードを作成する
Node *new_num_node(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// トークンを任意の個数読み進める
void skip_token(Token *tok, int count) {
  for (int i = 0; i < count; i++)
    *tok = *tok->next;
}

// トークンを1つ読み進める
void next_token(Token *tok) {
  skip_token(tok, 1);
}

// トークンが期待している記号のときには、真を返して
// トークンを1つ読み進める。それ以外の場合には偽を返す。
bool consume(Token *tok, TokenKind kind) {
  if (tok->kind != kind)
    return false;
  next_token(tok);
  return true;
}

// トークンが期待している記号であるか判定
bool consume_nostep(Token *tok, TokenKind kind) {
  if (tok->kind != kind)
    return false;
  return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(Token *tok,TokenKind kind) {
  if (tok->kind != kind)
    unexpected_symbol_error(tok->start, kind);
  next_token(tok);
}

// 次のトークンが期待しているトークンでなければエラーを報告する
void expect_nostep(Token *tok,TokenKind kind) {
  if (tok->kind != kind)
    unexpected_symbol_error(tok->start, kind);
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number(Token *tok) {
  if (tok->kind != TK_NUM)
    error_at(tok->start, "数ではありません");
  int val = tok->val;
  *tok = *tok->next;
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
    error_at(tok->start, "定義済みの変数または配列を再定義することはできません");
  } else { // 初登場のローカル変数の場合、localsの先頭に繋ぐ
    lvar = calloc(1, sizeof(Lvar));
    lvar->len = tok->len;
    if (cur_func->locals->type->kind == TYPE_ARRAY) {
      lvar->offset = cur_func->locals->offset + cur_func->locals->type->array_size * 8;
    } else {
      lvar->offset = cur_func->locals->offset + 8;
    }
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

Type *parse_type(Token *tok) {
  if (!consume(tok, TK_INT))
    return NULL;
  Type *type = calloc(1, sizeof(Type));
  type->kind = TYPE_INT;
  while (consume(tok, TK_ASTERISK)) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TYPE_PTR;
    ty->ptr_to = type;
    type = ty;
  }
  tok = tok->next; // 識別子を読み飛ばす
  if (consume_nostep(tok, TK_LEFT_BRACKET)) {
    tok = tok->next; // tokを数字に進める
    Type *ty_array = calloc(1, sizeof(Type));
    ty_array->kind = TYPE_ARRAY;
    ty_array->ptr_to = type;
    ty_array->array_size = tok->val;
    tok = tok->next; // tokを]に進める
    expect_nostep(tok, TK_RIGHT_BRACKET);
    return ty_array;
  }
  return type;
}

void init_func_declaration() {
  func_declaration_tail.ret_type = NULL;
  func_declaration_tail.name = "";
  func_declaration_tail.len = 0;
  func_declaration_list = &func_declaration_tail;
}

void new_func_declaration(Type *func_type, Token *func_name) {
  FuncDeclaration *func_dec = calloc(1, sizeof(FuncDeclaration));
  func_dec->ret_type = func_type;
  func_dec->name = calloc(func_name->len + 1, sizeof(char));
  memcpy(func_dec->name, func_name->start, func_name->len);
  func_dec->len = func_name->len;
  func_dec->next = func_declaration_list;
  func_declaration_list = func_dec;
}

void new_func_definition(Type *func_type, Token *func_name) {
  cur_func = cur_func->next = calloc(1, sizeof(Function));
  cur_func->type = func_type;
  cur_func->name = calloc(func_name->len + 1, sizeof(char));
  memcpy(cur_func->name, func_name->start, func_name->len);
  new_func_declaration(func_type, func_name);
  // ローカル変数
  Lvar lvar_tail;
  lvar_tail.len = 0;
  lvar_tail.name = NULL;
  lvar_tail.next = NULL;
  lvar_tail.offset = 0;
  cur_func->locals = &lvar_tail;
}

void parse_parameter(Function *func, Token *tok) {
  cur_func->params_head.next = NULL;
  cur_func->params_head.name = "";
  cur_func->params_head.len = 0;
  cur_func->params_head.offset = 0;
  if (!consume(tok, TK_RIGHT_PARENTHESIS)) {
    Lvar *cur_param = &cur_func->params_head;
    do {
      cur_param->next = calloc(1, sizeof(Lvar));
      cur_param->next->type = parse_type(tok); // parameterの型
      cur_param->next->name = calloc(tok->len + 1, sizeof(char));
      memcpy(cur_param->next->name, tok->start, tok->len);
      cur_param->next->len = tok->len;
      cur_param->next->offset = cur_param->offset + 8;
      cur_param = cur_param->next;
      next_token(tok);
    } while (consume(tok, TK_COMMA));
    cur_param->next = NULL;
    expect(tok, TK_RIGHT_PARENTHESIS);
  }

  // パラメータをローカル変数にコピー
  // TODO: 以下の処理をまとめた関数を作る
  for (Lvar *cur_param = cur_func->params_head.next; cur_param; cur_param = cur_param->next) {
    Lvar *new_lvar = calloc(1, sizeof(Lvar));
    new_lvar->name = calloc(cur_param->len + 1, sizeof(char));
    memcpy(new_lvar->name, cur_param->name, cur_param->len);
    new_lvar->len = cur_param->len;
    new_lvar->offset = cur_param->offset;
    new_lvar->type = cur_param->type;
    new_lvar->next = cur_func->locals;
    cur_func->locals = new_lvar;
  }
}

bool is_eof() {
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
  while (!is_eof()) {
    Type *func_type = parse_type(token);
    Token func_name = *token; // 関数名
    next_token(token);
    expect(token, TK_LEFT_PARENTHESIS);

    // 関数宣言の場合、宣言を記録し読み飛ばす
    if (consume_nostep(token, TK_RIGHT_PARENTHESIS) && consume_nostep(token->next, TK_SEMICOLON)) {
      new_func_declaration(func_type, &func_name);
      skip_token(token, 2);
    }

    // 関数定義の場合
    else {    
      new_func_definition(func_type, &func_name);
      parse_parameter(cur_func, token); // 引数
      expect(token, TK_LEFT_BRACE);
      cur_func->body = new_node(ND_STMT); // statement
      Node head;
      Node *cur_stmt = &head;
      while (!consume(token, TK_RIGHT_BRACE)) {
        cur_stmt = cur_stmt->next = new_node(ND_STMT);
        cur_stmt->body = stmt();
      }
      cur_func->body= head.next;
    }
  }
  // 関数のリストの末端
  cur_func->next = NULL;
}

Node *stmt() {
  Node *node;
  if (consume(token, TK_IF)) {
    node = new_node(ND_IF);
    expect(token, TK_LEFT_PARENTHESIS);
    node->cond = expr();
    expect(token, TK_RIGHT_PARENTHESIS);
    node->then = stmt();
    if (consume(token, TK_ELSE))
      node->els = stmt();
  }

  else if (consume(token, TK_RETURN)) {
    node = new_node(ND_RETURN);
    node->lhs = expr();
    expect(token, TK_SEMICOLON);
  }

  else if (consume(token, TK_WHILE)) {
    node = new_node(ND_FOR);
    expect(token, TK_LEFT_PARENTHESIS);
    node->cond = expr();
    expect(token, TK_RIGHT_PARENTHESIS);
    node->then = stmt();
  }

  else if (consume(token, TK_FOR)) {
    node = new_node(ND_FOR);
    expect(token, TK_LEFT_PARENTHESIS);
    if (*(token->start) != ';')
      node->init = expr();
    expect(token, TK_SEMICOLON);
    if (*(token->start) != ';')
      node->cond = expr();
    expect(token, TK_SEMICOLON);
    if (*(token->start) != ')')
      node->inc = expr();
    expect(token, TK_RIGHT_PARENTHESIS);
    node->then = stmt();
  }

  else if (consume(token, TK_LEFT_BRACE)) {
    node = new_node(ND_STMT);
    Node head;
    Node *cur = &head;
    while (!consume(token, TK_RIGHT_BRACE)) {
      cur = cur->next = new_node(ND_STMT);
      cur->body = stmt();
    }
    node = head.next;
  }
  
  else {
    node = expr();
    expect(token, TK_SEMICOLON);
  }
  
  return node;
}

Node *expr() {
  Type *ty = parse_type(token);
  if (ty != NULL) {
    // 配列の場合
    if (ty->kind == TYPE_ARRAY) {
      Node *node = new_lvar_node(ty, token);
      skip_token(token, 4);
      return node;
    // 通常の変数の場合
    } else {
      Node *node = new_lvar_node(ty, token);
      next_token(token);
      return node;
    }
  }
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume(token, TK_ASSIGN))
    node = new_binary_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  while (true) {
    if (consume(token, TK_EQUAL))
      node = new_binary_node(ND_EQ, node, relational());
    else if (consume(token, TK_NOT_EQUAL))
      node = new_binary_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  while (true) {
    if (consume(token, TK_LESS))
      node = new_binary_node(ND_LT, node, add());
    else if (consume(token, TK_LESS_EQUAL))
      node = new_binary_node(ND_LE, node, add());
    else if (consume(token, TK_GREATER))
      node = new_binary_node(ND_LT, add(), node);
    else if (consume(token, TK_GREATER_EQUAL))
      node = new_binary_node(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  while (true) {
    if (consume(token, TK_PLUS))
      node = new_binary_node(ND_ADD, node, mul());
    else if (consume(token, TK_MINUS))
      node = new_binary_node(ND_SUB, node, mul());
    else
      return node; // 数値のみの場合
  }
}

Node *mul() {
  Node *node = unary();

  while (true) {
    if (consume(token, TK_ASTERISK))
      node = new_binary_node(ND_MUL, node, unary());
    else if (consume(token, TK_SLASH))
      node = new_binary_node(ND_DIV, node, unary());
    else
      return node; // 数値のみの場合
  }
}

Node *unary() {
  if (consume(token, TK_SIZEOF)) {
    Node *node = new_node(ND_SIZEOF);
    node->lhs = unary();
    return node;
  }
  if (consume(token, TK_PLUS))
    return new_binary_node(ND_ADD, new_num_node(0), primary());
  if (consume(token, TK_MINUS))
    return new_binary_node(ND_SUB, new_num_node(0), primary());
  if (consume(token, TK_ASTERISK)) {
    Node *node = new_node(ND_DEREF);
    node->lhs = unary();
    return node;
  }
  if (consume(token, TK_AND)) {
    Node *node = new_node(ND_ADDR);
    node->lhs = unary();
    return node;
  }
  return primary();
}

Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume(token, TK_LEFT_PARENTHESIS)) {
    Node *node = expr();
    expect(token, TK_RIGHT_PARENTHESIS);
    return node;
  }

  else if (token->kind == TK_IDENT) {
    Node *node;
    Token *ident = token;
    token = token->next;

    if (consume(token, TK_LEFT_PARENTHESIS)) { // 関数呼び出しの場合
      node = new_node(ND_FUNCALL);
      node->func_name = calloc(ident->len + 1, sizeof(char));
      memcpy(node->func_name, ident->start, ident->len);

      if (!consume(token, TK_RIGHT_PARENTHESIS)) {
        Node head;
        Node *cur = &head;
        do {
          cur = cur->next = new_node(ND_EXPR);
          cur->body = expr();
        } while (consume(token, TK_COMMA));
        expect(token, TK_RIGHT_PARENTHESIS);
        node->expr = head.next;
      }
    }
    else if (consume(token, TK_LEFT_BRACKET)) { // 配列の場合
      node = new_node(ND_DEREF);
      node->lhs = new_node(ND_ADD);
      node->lhs->lhs = lvar_node(ident);
      node->lhs->rhs = expr();
      expect(token, TK_RIGHT_BRACKET);
    }
    else { // ローカル変数の場合
      node = lvar_node(ident);
    }
    return node;
  }

  else
    return new_num_node(expect_number(token)); // それ以外は整数のはず
}

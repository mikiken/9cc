#include "9cc.h"

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
  next_token(tok);
  return val;
}

Lvar *find_lvar(Function *func, Token *tok) {
  for (Lvar *var = func->lvar_list; var != NULL; var = var->next)
    if (var->len == tok->len && !memcmp(tok->start, var->name, var->len))
      return var;
  return NULL;
}

int base_type_size(Type *type) {
  switch (type->kind) {
    case TYPE_INT:
      return SIZE_INT;
    case TYPE_PTR:
      return SIZE_PTR;
    default:
      error("未対応の型です");
  }
}

int lvar_offset(int cur_offset, int size) {
  if (cur_offset - cur_offset / 8 * 8 + size <= 8)
    return cur_offset + size;
  else
    return (cur_offset / size + 1) * size + size;
}

int calc_lvar_offset(Lvar *lvar_list, Type *type) {
  int cur_offset = 0;
  if (lvar_list->type->kind == TYPE_ARRAY)
    cur_offset = lvar_list->offset + base_type_size(lvar_list->type->ptr_to) * (lvar_list->type->array_size - 1);
  else
    cur_offset = lvar_list->offset;
    
  switch (type->kind) {
    case TYPE_INT:
      return lvar_offset(cur_offset, SIZE_INT);
    case TYPE_PTR:
      return lvar_offset(cur_offset, SIZE_PTR);
    case TYPE_ARRAY:
      return lvar_offset(cur_offset, base_type_size(type->ptr_to));
    default:
      error("未対応の型です");
  }
}

Node *new_lvar_definition(Function *func, Type *type, Token *tok) {
  Node *node = new_node(ND_LVARDEF);
  Lvar *lvar = find_lvar(func, tok);
  
  if (lvar != NULL) {
    error_at(tok->start, "定義済みの変数または配列を再定義することはできません");
  } else { // 初登場のローカル変数の場合、localsの先頭に繋ぐ
    lvar = calloc(1, sizeof(Lvar));
    lvar->len = tok->len;
    lvar->offset = calc_lvar_offset(func->lvar_list, type);
    lvar->name = tok->start;
    lvar->type = type;
    lvar->next = func->lvar_list;
    func->lvar_list = lvar;
    
    node->offset = lvar->offset;
  }
  return node;
}

Node *lvar_node(Function *func, Token *tok) {
  Node *node = new_node(ND_LVAR);
  Lvar *lvar = find_lvar(func, tok);

  if (lvar == NULL) {
    error_at(tok->start, "未定義の変数です");
  } else {
    node->offset = lvar->offset;
  }
  return node;
}

Type *parse_base_type(Token *tok) {
  Type *base_type = calloc(1, sizeof(Type));
  if (consume(tok, TK_INT)) {
    base_type->kind = TYPE_INT;
  } else {
    base_type->kind = TYPE_NULL;
  }
  return base_type;
}

Type *parse_type(Token *tok) {
  Type *type = parse_base_type(tok);
  if (type->kind == TYPE_NULL)
    return NULL;
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

Function *init_func_head() {
  Function *func_head = calloc(1, sizeof(Function));
  return func_head;
}

void init_lvar_list(Function *func) {
  Lvar *lvar_tail = calloc(1, sizeof(Lvar));
  lvar_tail->next = NULL;
  lvar_tail->name = "";
  lvar_tail->len = 0;
  lvar_tail->offset = 0;
  lvar_tail->type = calloc(1, sizeof(Type)); // lvar_tailに型情報を付与
  lvar_tail->type->kind = TYPE_NULL;
  func->lvar_list = lvar_tail;
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

Function *new_func_definition(Type *func_type, Token *func_name) {
  Function *func = calloc(1, sizeof(Function));
  func->type = func_type;
  func->name = calloc(func_name->len + 1, sizeof(char));
  memcpy(func->name, func_name->start, func_name->len);
  init_lvar_list(func);
  new_func_declaration(func_type, func_name);
  return func;
}

// 関数の引数をローカル変数にコピー
void copy_param_to_lvar(Function *func) {
  for (Lvar *cur_param = func->params_head.next; cur_param; cur_param = cur_param->next) {
    Lvar *new_lvar = calloc(1, sizeof(Lvar));
    new_lvar->name = calloc(cur_param->len + 1, sizeof(char));
    memcpy(new_lvar->name, cur_param->name, cur_param->len);
    new_lvar->len = cur_param->len;
    new_lvar->offset = cur_param->offset;
    new_lvar->type = cur_param->type;
    new_lvar->next = func->lvar_list;
    func->lvar_list = new_lvar;
  }
}

Lvar *init_params_head(Function *func) {
  func->params_head.next = NULL;
  func->params_head.type = calloc(1, sizeof(Type));
  func->params_head.type->kind = TYPE_NULL;
  func->params_head.name = "";
  func->params_head.len = 0;
  func->params_head.offset = 0;
  return &func->params_head;
}

void parse_parameter(Function *func, Token *tok) {
  if (!consume(tok, TK_RIGHT_PARENTHESIS)) {
    Lvar *cur_param = init_params_head(func);
    do {
      cur_param->next = calloc(1, sizeof(Lvar));
      cur_param->next->type = parse_type(tok); // parameterの型
      cur_param->next->name = calloc(tok->len + 1, sizeof(char));
      memcpy(cur_param->next->name, tok->start, tok->len);
      cur_param->next->len = tok->len;
      cur_param->next->offset = calc_lvar_offset(cur_param, cur_param->next->type);
      cur_param = cur_param->next;
      next_token(tok);
    } while (consume(tok, TK_COMMA));
    cur_param->next = NULL;
    copy_param_to_lvar(func); // 引数をローカル変数としてコピー
    expect(tok, TK_RIGHT_PARENTHESIS);
  }
}

bool is_eof(Token *tok) {
  return tok->kind == TK_EOF;
}

Function *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Function *parse(Token *tok) {
  init_func_declaration();
  return program(tok);
}

Function *program(Token *tok) {
  Function *func_head = init_func_head();
  Function *cur_func = func_head;
  while (!is_eof(tok)) {
    Type *func_type = parse_type(tok); // 関数の返り値の型
    Token func_name = *tok; // 関数名
    next_token(tok);
    expect(tok, TK_LEFT_PARENTHESIS);

    // 関数宣言の場合、宣言を記録し読み飛ばす
    if (consume_nostep(tok, TK_RIGHT_PARENTHESIS) && consume_nostep(tok->next, TK_SEMICOLON)) {
      new_func_declaration(func_type, &func_name);
      skip_token(tok, 2);
    }

    // 関数定義の場合
    else {    
      cur_func = cur_func->next = new_func_definition(func_type, &func_name);
      parse_parameter(cur_func, tok); // 引数
      expect(tok, TK_LEFT_BRACE);
      cur_func->body = new_node(ND_STMT); // statement
      Node head;
      Node *cur_stmt = &head;
      while (!consume(tok, TK_RIGHT_BRACE)) {
        cur_stmt = cur_stmt->next = new_node(ND_STMT);
        cur_stmt->body = stmt(cur_func, tok);
      }
      cur_func->body= head.next;
    }
  }
  // 関数のリストの末端
  cur_func->next = NULL;
  return func_head->next;
}

Node *stmt(Function *func, Token *tok) {
  Node *node;
  if (consume(tok, TK_IF)) {
    node = new_node(ND_IF);
    expect(tok, TK_LEFT_PARENTHESIS);
    node->cond = expr(func, tok);
    expect(tok, TK_RIGHT_PARENTHESIS);
    node->then = stmt(func, tok);
    if (consume(tok, TK_ELSE))
      node->els = stmt(func, tok);
  }

  else if (consume(tok, TK_RETURN)) {
    node = new_node(ND_RETURN);
    node->lhs = expr(func, tok);
    expect(tok, TK_SEMICOLON);
  }

  else if (consume(tok, TK_WHILE)) {
    node = new_node(ND_FOR);
    expect(tok, TK_LEFT_PARENTHESIS);
    node->cond = expr(func, tok);
    expect(tok, TK_RIGHT_PARENTHESIS);
    node->then = stmt(func, tok);
  }

  else if (consume(tok, TK_FOR)) {
    node = new_node(ND_FOR);
    expect(tok, TK_LEFT_PARENTHESIS);
    if (!consume_nostep(tok, TK_SEMICOLON))
      node->init = expr(func, tok);
    expect(tok, TK_SEMICOLON);
    if (!consume_nostep(tok, TK_SEMICOLON))
      node->cond = expr(func, tok);
    expect(tok, TK_SEMICOLON);
    if (!consume_nostep(tok, TK_RIGHT_PARENTHESIS))
      node->inc = expr(func, tok);
    expect(tok, TK_RIGHT_PARENTHESIS);
    node->then = stmt(func, tok);
  }

  else if (consume(tok, TK_LEFT_BRACE)) {
    node = new_node(ND_STMT);
    Node head;
    Node *cur = &head;
    while (!consume(tok, TK_RIGHT_BRACE)) {
      cur = cur->next = new_node(ND_STMT);
      cur->body = stmt(func, tok);
    }
    node = head.next;
  }
  
  else {
    node = expr(func, tok);
    expect(tok, TK_SEMICOLON);
  }
  
  return node;
}

Node *expr(Function *func, Token *tok) {
  Type *ty = parse_type(tok);
  if (ty != NULL) {
    Node *node = new_lvar_definition(func, ty, tok);
    // 配列の場合
    if (ty->kind == TYPE_ARRAY)
      skip_token(tok, 4);
    // 通常の変数の場合
    else
      next_token(tok);
    return node;
  }
  return assign(func, tok);
}

Node *assign(Function *func, Token *tok) {
  Node *node = equality(func, tok);
  if (consume(tok, TK_ASSIGN))
    node = new_binary_node(ND_ASSIGN, node, assign(func, tok));
  return node;
}

Node *equality(Function *func, Token *tok) {
  Node *node = relational(func, tok);

  while (true) {
    if (consume(tok, TK_EQUAL))
      node = new_binary_node(ND_EQ, node, relational(func, tok));
    else if (consume(tok, TK_NOT_EQUAL))
      node = new_binary_node(ND_NE, node, relational(func, tok));
    else
      return node;
  }
}

Node *relational(Function *func, Token *tok) {
  Node *node = add(func, tok);

  while (true) {
    if (consume(tok, TK_LESS))
      node = new_binary_node(ND_LT, node, add(func, tok));
    else if (consume(tok, TK_LESS_EQUAL))
      node = new_binary_node(ND_LE, node, add(func, tok));
    else if (consume(tok, TK_GREATER))
      node = new_binary_node(ND_LT, add(func, tok), node);
    else if (consume(tok, TK_GREATER_EQUAL))
      node = new_binary_node(ND_LE, add(func, tok), node);
    else
      return node;
  }
}

Node *add(Function *func, Token *tok) {
  Node *node = mul(func, tok);

  while (true) {
    if (consume(tok, TK_PLUS))
      node = new_binary_node(ND_ADD, node, mul(func, tok));
    else if (consume(tok, TK_MINUS))
      node = new_binary_node(ND_SUB, node, mul(func, tok));
    else
      return node; // 数値のみの場合
  }
}

Node *mul(Function *func, Token *tok) {
  Node *node = unary(func, tok);

  while (true) {
    if (consume(tok, TK_ASTERISK))
      node = new_binary_node(ND_MUL, node, unary(func, tok));
    else if (consume(tok, TK_SLASH))
      node = new_binary_node(ND_DIV, node, unary(func, tok));
    else
      return node; // 数値のみの場合
  }
}

Node *unary(Function *func, Token *tok) {
  if (consume(tok, TK_SIZEOF)) {
    Node *node = new_node(ND_SIZEOF);
    node->lhs = unary(func, tok);
    return node;
  }
  if (consume(tok, TK_PLUS))
    return new_binary_node(ND_ADD, new_num_node(0), primary(func, tok));
  if (consume(tok, TK_MINUS))
    return new_binary_node(ND_SUB, new_num_node(0), primary(func, tok));
  if (consume(tok, TK_ASTERISK)) {
    Node *node = new_node(ND_DEREF);
    node->lhs = unary(func, tok);
    return node;
  }
  if (consume(tok, TK_AND)) {
    Node *node = new_node(ND_ADDR);
    node->lhs = unary(func, tok);
    return node;
  }
  return primary(func, tok);
}

Node *primary(Function *func, Token *tok) {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume(tok, TK_LEFT_PARENTHESIS)) {
    Node *node = expr(func, tok);
    expect(tok, TK_RIGHT_PARENTHESIS);
    return node;
  }

  else if (tok->kind == TK_IDENT) {
    Node *node;
    Token ident = *tok;
    next_token(tok);

    if (consume(tok, TK_LEFT_PARENTHESIS)) { // 関数呼び出しの場合
      node = new_node(ND_FUNCALL);
      node->func_name = calloc(ident.len + 1, sizeof(char));
      memcpy(node->func_name, ident.start, ident.len);

      if (!consume(tok, TK_RIGHT_PARENTHESIS)) {
        Node head;
        Node *cur = &head;
        do {
          cur = cur->next = new_node(ND_EXPR);
          cur->body = expr(func, tok);
        } while (consume(tok, TK_COMMA));
        expect(tok, TK_RIGHT_PARENTHESIS);
        node->expr = head.next;
      }
    }
    else if (consume(tok, TK_LEFT_BRACKET)) { // 配列の場合
      node = new_node(ND_DEREF);
      node->lhs = new_node(ND_ADD);
      node->lhs->lhs = lvar_node(func, &ident);
      node->lhs->rhs = expr(func, tok);
      expect(tok, TK_RIGHT_BRACKET);
    }
    else { // ローカル変数の場合
      node = lvar_node(func, &ident);
    }
    return node;
  }

  else
    return new_num_node(expect_number(tok)); // それ以外は整数のはず
}

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
void expect(Token *tok, TokenKind kind) {
  if (tok->kind != kind)
    unexpected_symbol_error(tok->start, kind);
  next_token(tok);
}

// 次のトークンが期待しているトークンでなければエラーを報告する
void expect_nostep(Token *tok, TokenKind kind) {
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

Obj *find_var(Obj *var_list, Token *var_name) {
  for (Obj *v = var_list; v != NULL; v = v->next)
    if (v->len == var_name->len && !memcmp(var_name->start, v->name, v->len))
      return v;
  return NULL;
}

int base_type_size(Type *type) {
  switch (type->kind) {
    case TYPE_INT:
      return SIZE_INT;
    case TYPE_CHAR:
      return SIZE_CHAR;
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

int calc_lvar_offset(Obj *lvar_list, Type *type) {
  switch (type->kind) {
    case TYPE_INT:
      return lvar_offset(lvar_list->offset, SIZE_INT);
    case TYPE_CHAR:
      return lvar_offset(lvar_list->offset, SIZE_CHAR);
    case TYPE_PTR:
      return lvar_offset(lvar_list->offset, SIZE_PTR);
    case TYPE_ARRAY:
      return lvar_offset(lvar_list->offset, base_type_size(type->ptr_to)) + base_type_size(type->ptr_to) * (type->array_size - 1);
    default:
      error("未対応の型です");
  }
}

Node *new_lvar_definition(Function *func, Type *type, Token *tok) {
  Node *node = new_node(ND_LVARDEF);
  Obj *lvar = find_var(func->lvar_list, tok);

  if (lvar != NULL) {
    error_at(tok->start, "定義済みの変数または配列を再定義することはできません");
  }
  else { // 初登場のローカル変数の場合、localsの先頭に繋ぐ
    lvar = calloc(1, sizeof(Obj));
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

Node *var_node(Function *func, Token *var_name) {
  Node *node;
  Obj *lvar = find_var(func->lvar_list, var_name);
  // ローカル変数の場合
  if (lvar != NULL) {
    node = new_node(ND_LVAR);
    node->offset = lvar->offset;
  }
  // グローバル変数の場合
  else {
    Obj *gvar = find_var(global_var_list, var_name);
    if (gvar == NULL)
      error_at(var_name->start, "未定義の変数です");
    node = new_node(ND_GVAR);
    node->gvar_name = calloc(var_name->len + 1, sizeof(char));
    memcpy(node->gvar_name, var_name->start, var_name->len);
  }
  return node;
}

Type *parse_base_type(Token *tok) {
  Type *base_type = calloc(1, sizeof(Type));
  if (consume(tok, TK_INT))
    base_type->kind = TYPE_INT;
  else if (consume(tok, TK_CHAR))
    base_type->kind = TYPE_CHAR;
  else if (consume(tok, TK_VOID))
    base_type->kind = TYPE_VOID;
  else
    base_type->kind = TYPE_NULL;
  return base_type;
}

Type *parse_type(Token *tok) {
  Type *type = parse_base_type(tok);
  if (type->kind == TYPE_NULL)
    return NULL;
  // ポインタ型の場合
  while (consume(tok, TK_ASTERISK)) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TYPE_PTR;
    ty->ptr_to = type;
    type = ty;
  }
  tok = tok->next; // 識別子を読み飛ばす
  // 配列型の場合
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
  Obj *lvar_tail = calloc(1, sizeof(Obj));
  lvar_tail->next = NULL;
  lvar_tail->name = "";
  lvar_tail->len = 0;
  lvar_tail->offset = 0;
  lvar_tail->type = calloc(1, sizeof(Type)); // lvar_tailに型情報を付与
  lvar_tail->type->kind = TYPE_NULL;
  func->lvar_list = lvar_tail;
}

void init_global_var_list() {
  global_var_tail.type = NULL;
  global_var_tail.name = "";
  global_var_tail.len = 0;
  global_var_list = &global_var_tail;
}

void new_global_var(Type *gvar_type, Token *gvar_name) {
  Obj *new_gvar = calloc(1, sizeof(Obj));
  new_gvar->type = gvar_type;
  new_gvar->name = calloc(gvar_name->len + 1, sizeof(char));
  memcpy(new_gvar->name, gvar_name->start, gvar_name->len);
  new_gvar->len = gvar_name->len;
  new_gvar->next = global_var_list;
  global_var_list = new_gvar;
}

// 文字列リテラルのid用
int str_count;

Obj *new_string_literal(Token *str) {
  Obj *new_str = calloc(1, sizeof(Obj));
  new_str->type = new_type(TYPE_CHAR); // ここ自信ない
  new_str->init_data = calloc(str->len + 1, sizeof(char));
  memcpy(new_str->init_data, str->start, str->len);
  new_str->len = str->len;
  new_str->str_id = str_count++;
  new_str->next = global_var_list;
  global_var_list = new_str;
  return new_str;
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
  for (Obj *cur_param = func->params_head.next; cur_param; cur_param = cur_param->next) {
    Obj *new_lvar = calloc(1, sizeof(Obj));
    new_lvar->name = calloc(cur_param->len + 1, sizeof(char));
    memcpy(new_lvar->name, cur_param->name, cur_param->len);
    new_lvar->len = cur_param->len;
    new_lvar->offset = cur_param->offset;
    new_lvar->type = cur_param->type;
    new_lvar->next = func->lvar_list;
    func->lvar_list = new_lvar;
  }
}

Obj *init_params_head(Function *func) {
  func->params_head.next = NULL;
  func->params_head.type = calloc(1, sizeof(Type));
  func->params_head.type->kind = TYPE_NULL;
  func->params_head.name = "";
  func->params_head.len = 0;
  func->params_head.offset = 0;
  return &func->params_head;
}

void parse_parameter(Function *func, Token *tok) {
  // func_name(void) の場合
  if (consume(tok, TK_VOID)) {
    expect(tok, TK_RIGHT_PARENTHESIS);
    return;
  }
  // func_name() の場合
  else if (consume(tok, TK_RIGHT_PARENTHESIS))
    return;
  // 引数ありの場合
  else {
    Obj *cur_param = init_params_head(func);
    do {
      cur_param->next = calloc(1, sizeof(Obj));
      Type *type = parse_type(tok);
      if (type->kind == TYPE_VOID)
        error_at(tok->start, "void型の引数を定義することはできません");
      cur_param->next->type = type; // parameterの型
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
Node *conditional();
Node *logical_or();
Node *logical_and();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
Node *primary();

Function *parse(Token *tok) {
  init_func_declaration();
  init_global_var_list();
  return program(tok);
}

Function *program(Token *tok) {
  Function *func_head = init_func_head();
  Function *cur_func = func_head;
  while (!is_eof(tok)) {
    Type *ident_type = parse_type(tok); // 関数/グローバル変数の型
    Token ident_name = *tok;            // 関数/グローバル変数名
    next_token(tok);

    // 関数の場合
    if (consume(tok, TK_LEFT_PARENTHESIS)) {
      // 関数宣言の場合、宣言を記録し読み飛ばす
      if (consume_nostep(tok, TK_RIGHT_PARENTHESIS) && consume_nostep(tok->next, TK_SEMICOLON)) {
        new_func_declaration(ident_type, &ident_name);
        skip_token(tok, 2);
      }
      // 関数定義の場合
      else {
        cur_func = cur_func->next = new_func_definition(ident_type, &ident_name);
        parse_parameter(cur_func, tok); // 引数
        expect(tok, TK_LEFT_BRACE);
        cur_func->body = new_node(ND_STMT); // statement
        Node head;
        Node *cur_stmt = &head;
        while (!consume(tok, TK_RIGHT_BRACE)) {
          cur_stmt = cur_stmt->next = new_node(ND_STMT);
          cur_stmt->body = stmt(cur_func, tok);
        }
        cur_func->body = head.next;
      }
    }
    // グローバル変数の場合
    else {
      new_global_var(ident_type, &ident_name);
      if (ident_type->kind == TYPE_ARRAY)
        skip_token(tok, 3);
      expect(tok, TK_SEMICOLON);
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
    // return; となる場合 (void型)
    if (consume(tok, TK_SEMICOLON)) {
      return node;
    }
    else {
      node->lhs = expr(func, tok);
      expect(tok, TK_SEMICOLON);
    }
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
    if (ty->kind == TYPE_VOID)
      error_at(tok->start, "void型の変数を定義することはできません");
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
  Node *node = conditional(func, tok);
  if (consume(tok, TK_ASSIGN))
    node = new_binary_node(ND_ASSIGN, node, assign(func, tok));
  else if (consume(tok, TK_ADD_ASSIGN))
    node = new_binary_node(ND_ASSIGN, node, new_binary_node(ND_ADD, node, assign(func, tok)));
  else if (consume(tok, TK_SUB_ASSIGN))
    node = new_binary_node(ND_ASSIGN, node, new_binary_node(ND_SUB, node, assign(func, tok)));
  else if (consume(tok, TK_MUL_ASSIGN))
    node = new_binary_node(ND_ASSIGN, node, new_binary_node(ND_MUL, node, assign(func, tok)));
  else if (consume(tok, TK_DIV_ASSIGN))
    node = new_binary_node(ND_ASSIGN, node, new_binary_node(ND_DIV, node, assign(func, tok)));
  else if (consume(tok, TK_MOD_ASSIGN))
    node = new_binary_node(ND_ASSIGN, node, new_binary_node(ND_MOD, node, assign(func, tok)));
  return node;
}

Node *conditional(Function *func, Token *tok) {
  Node *cond = logical_or(func, tok);

  // conditional operator
  if (consume(tok, TK_QUESTION)) {
    Node *node = new_node(ND_COND);
    node->cond = cond;
    node->then = expr(func, tok);
    expect(tok, TK_COLON);
    node->els = conditional(func, tok);
    return node;
  }
  else {
    return cond;
  }
}

Node *logical_or(Function *func, Token *tok) {
  Node *node = logical_and(func, tok);

  while (true) {
    if (consume(tok, TK_LOGICAL_OR))
      node = new_binary_node(ND_OR, node, logical_and(func, tok));
    else
      return node;
  }
}

Node *logical_and(Function *func, Token *tok) {
  Node *node = equality(func, tok);

  while (true) {
    if (consume(tok, TK_LOGICAL_AND))
      node = new_binary_node(ND_AND, node, equality(func, tok));
    else
      return node;
  }
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
      return node;
  }
}

Node *mul(Function *func, Token *tok) {
  Node *node = unary(func, tok);

  while (true) {
    if (consume(tok, TK_ASTERISK))
      node = new_binary_node(ND_MUL, node, unary(func, tok));
    else if (consume(tok, TK_SLASH))
      node = new_binary_node(ND_DIV, node, unary(func, tok));
    else if (consume(tok, TK_PERCENT))
      node = new_binary_node(ND_MOD, node, unary(func, tok));
    else
      return node;
  }
}

Node *unary(Function *func, Token *tok) {
  if (consume(tok, TK_SIZEOF)) {
    Node *node = new_node(ND_SIZEOF);
    node->lhs = unary(func, tok);
    return node;
  }
  else if (consume(tok, TK_PLUS))
    return new_binary_node(ND_ADD, new_num_node(0), unary(func, tok));
  else if (consume(tok, TK_MINUS))
    return new_binary_node(ND_SUB, new_num_node(0), unary(func, tok));
  else if (consume(tok, TK_ASTERISK)) {
    Node *node = new_node(ND_DEREF);
    node->lhs = unary(func, tok);
    return node;
  }
  else if (consume(tok, TK_AND)) {
    Node *node = new_node(ND_ADDR);
    node->lhs = unary(func, tok);
    return node;
  }
  else if (consume(tok, TK_INCREMENT)) {
    Node *lhs = unary(func, tok);
    return new_binary_node(ND_ASSIGN, lhs, new_binary_node(ND_ADD, lhs, new_num_node(1)));
  }
  else if (consume(tok, TK_DECREMENT)) {
    Node *lhs = unary(func, tok);
    return new_binary_node(ND_ASSIGN, lhs, new_binary_node(ND_SUB, lhs, new_num_node(1)));
  }
  else if (consume(tok, TK_LOGICAL_NOT)) {
    Node *node = new_node(ND_NOT);
    node->lhs = unary(func, tok);
    return node;
  }
  return postfix(func, tok);
}

Node *postfix(Function *func, Token *tok) {
  Node *node = primary(func, tok);
  if (consume(tok, TK_LEFT_BRACKET)) {
    node = new_binary_node(ND_DEREF, new_binary_node(ND_ADD, node, expr(func, tok)), NULL);
    expect(tok, TK_RIGHT_BRACKET);
  }
  // (a += 1) - 1としてパース
  else if (consume(tok, TK_INCREMENT))
    node = new_binary_node(ND_SUB, new_binary_node(ND_ASSIGN, node, new_binary_node(ND_ADD, node, new_num_node(1))), new_num_node(1));
  // (a -= 1) + 1としてパース
  else if (consume(tok, TK_DECREMENT))
    node = new_binary_node(ND_ADD, new_binary_node(ND_ASSIGN, node, new_binary_node(ND_SUB, node, new_num_node(1))), new_num_node(1));
  return node;
}

Node *primary(Function *func, Token *tok) {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume(tok, TK_LEFT_PARENTHESIS)) {
    Node *node = expr(func, tok);
    expect(tok, TK_RIGHT_PARENTHESIS);
    return node;
  }

  else if (consume_nostep(tok, TK_IDENT)) {
    Node *node;
    Token ident = *tok;
    next_token(tok);

    // 関数呼び出しの場合
    if (consume(tok, TK_LEFT_PARENTHESIS)) {
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
    // 変数の場合
    else {
      node = var_node(func, &ident);
    }
    return node;
  }

  // 文字列リテラルの場合
  else if (consume_nostep(tok, TK_STR)) {
    Node *node = new_node(ND_STR);
    node->str_id = new_string_literal(tok)->str_id;
    next_token(tok);
    return node;
  }

  // 整数の場合
  else if (consume_nostep(tok, TK_NUM)) {
    return new_num_node(expect_number(tok));
  }

  else {
    error_at(tok->start, "パースすることができませんでした");
  }
}

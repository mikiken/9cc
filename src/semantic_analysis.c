#include "9cc.h"

FuncDeclaration *find_declaration_by_name(char *name) {
  for (FuncDeclaration *declaration = func_declaration_list; declaration; declaration = declaration->next) {
    if (!name) {
      error("nameがNULLポインタです");
    }
    if (!declaration->name) {
      error("declaration->nameがNULLポインタです");
    }
    //名前が一致している場合
    if (!strcmp(declaration->name, name)) {
      return declaration;
    }
  }
  return NULL;
}

Obj *find_gvar_by_name(char *name) {
  for (Obj *gvar = global_var_list; gvar != NULL; gvar = gvar->next) {
    // 文字列リテラルは飛ばす
    if (gvar->init_data)
      continue;
    if (!name) {
      error("nameがNULLポインタです");
    }
    // 文字列リテラルでなく、グローバル変数の名前がNULLの場合はエラーにする
    if (!gvar->init_data && !gvar->name) {
      error("gvar->nameがNULLポインタです");
    }
    //名前が一致している場合
    if (!strcmp(gvar->name, name)) {
      return gvar;
    }
  }
  return NULL;
}

Obj *find_str_by_id(int id) {
  for (Obj *str = global_var_list; str != NULL; str = str->next) {
    if (!str->init_data)
      continue;
    //idが一致している場合
    if (str->str_id == id)
      return str;
  }
  return NULL;
}

Obj *find_lvar_by_offset(Obj *lvar_list, int offset) {
  for (Obj *lvar = lvar_list; lvar; lvar = lvar->next) {
    if (lvar->offset == offset) {
      return lvar;
    }
  }
  return NULL;
}

// 変数の型を返す関数
Type *var_type(Obj *var) {
  return var->type;
}

Type *new_type(TypeKind kind) {
  Type *type = calloc(1, sizeof(Type));
  type->kind = kind;
}

// 型なしnodeに型を追加した新たなnodeを返す
Node *new_typed_node(Type *type, Node *node) {
  Node *node_typed = calloc(1, sizeof(Node));
  *node_typed = *node;     // nodeをコピー
  node_typed->type = type; // 型情報を付加
  return node_typed;
}

Node *new_typed_binary(Node *node_typed, Node *lhs_typed, Node *rhs_typed) {
  node_typed->lhs = lhs_typed;
  node_typed->rhs = rhs_typed;
  return node_typed;
}

// ポインタ演算の調整用に型のサイズに合わせた整数nodeを返す
Node *new_size_node(Type *type) {
  Node *size = new_typed_node(new_type(TYPE_INT), new_node(ND_NUM));
  size->val = type_size(type);
  return size;
}

Node *cast_array_to_pointer(Node *array_node) {
  Node *ref_to_array = new_node(ND_ADDR);
  Type *ty_addr = new_type(TYPE_PTR);
  ty_addr->ptr_to = array_node->type->ptr_to; // 配列の型はptr_toに入っている
  ty_addr->array_size = array_node->type->array_size;
  return new_typed_binary(new_typed_node(ty_addr, ref_to_array), array_node, NULL);
}

bool is_arithmeric_type(Type *type) {
  return type->kind == TYPE_INT || type->kind == TYPE_CHAR;
}

bool is_array_type_node(Node *node) {
  return (node->kind == ND_LVAR || node->kind == ND_GVAR) && node->type->kind == TYPE_ARRAY;
}

bool is_ptr_type(Type *type) {
  return type->kind == TYPE_PTR;
}

TypeKind larger_arithmetic_type(Type *ty_1, Type *ty_2) {
  if (!is_arithmeric_type(ty_1) || !is_arithmeric_type(ty_2))
    error("算術型ではありません");
  return type_size(ty_1) >= type_size(ty_2) ? ty_1->kind : ty_2->kind;
}

Node *add_type_to_node(Function *function, Node *node) {
  switch (node->kind) {
    case ND_STMT: {
      Node typed_stmt_head;
      Node *typed_stmt = &typed_stmt_head;
      for (Node *n = node; n; n = n->next) {
        typed_stmt = typed_stmt->next = new_node(ND_STMT);
        typed_stmt->body = add_type_to_node(function, n->body);
      }
      return typed_stmt_head.next;
    }
    case ND_EXPR: {
      Node typed_expr_head;
      Node *typed_expr = &typed_expr_head;
      for (Node *n = node; n; n = n->next) {
        typed_expr = typed_expr->next = new_node(ND_EXPR);
        typed_expr->body = add_type_to_node(function, n->body);
      }
      return typed_expr_head.next;
    }
    case ND_COMMA: {
      Node *lhs = add_type_to_node(function, node->lhs);
      Node *rhs = add_type_to_node(function, node->rhs);
      return new_typed_binary(new_typed_node(rhs->type, node), lhs, rhs);
    }
    case ND_ADD:
    case ND_SUB:
    case ND_ASSIGN: {
      // ND_ADD や ND_SUB でポインタ演算の際に ptr_to の型に合わせて offset を sizeof(ptr_to) 倍する処理は  で行う
      // node自体の型は一旦 lhs の型に合わせておく
      Node *lhs = add_type_to_node(function, node->lhs);
      Node *rhs = add_type_to_node(function, node->rhs);
      return new_typed_binary(new_typed_node(lhs->type, node), lhs, rhs);
    }
    case ND_MUL:
    case ND_DIV:
    case ND_MOD:
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_AND:
    case ND_OR: {
      Node *lhs = add_type_to_node(function, node->lhs);
      Node *rhs = add_type_to_node(function, node->rhs);
      return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, rhs);
    }
    case ND_NOT: {
      Node *lhs = add_type_to_node(function, node->lhs);
      return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, NULL);
    }
    case ND_COND: {
      Node *cond = add_type_to_node(function, node->cond);
      Node *then = add_type_to_node(function, node->then);
      Node *els = add_type_to_node(function, node->els);
      Node *typed_node = new_typed_node(new_type(larger_arithmetic_type(then->type, els->type)), node);
      typed_node->cond = cond;
      typed_node->then = then;
      typed_node->els = els;
      return typed_node;
    }
    case ND_NUM:
      return new_typed_node(new_type(TYPE_INT), node);
    case ND_STR: {
      Node *ref_to_str = new_typed_node(new_type(TYPE_PTR), new_node(ND_ADDR));
      ref_to_str->type->ptr_to = new_type(TYPE_CHAR);
      ref_to_str->lhs = new_typed_node(new_type(TYPE_CHAR), node);
      return ref_to_str;
    }
    case ND_LVARDEF:
    case ND_LVAR:
      return new_typed_node(find_lvar_by_offset(function->lvar_list, node->offset)->type, node);
    case ND_GVAR:
      return new_typed_node(find_gvar_by_name(node->gvar_name)->type, node);
    case ND_RETURN: {
      // return <式>; の場合
      if (node->lhs) {
        Node *lhs = add_type_to_node(function, node->lhs);
        return new_typed_binary(new_typed_node(lhs->type, node), lhs, NULL);
      }
      // return; の場合
      else
        return new_typed_node(new_type(TYPE_NULL), node);
    }
    case ND_IF: {
      Node *typed_node = new_typed_node(new_type(TYPE_NULL), node);
      Node *cond = add_type_to_node(function, node->cond);
      Node *then = add_type_to_node(function, node->then);
      if (node->els) {
        Node *els = add_type_to_node(function, node->els);
        typed_node->els = els;
      }
      typed_node->cond = cond;
      typed_node->then = then;
      return typed_node;
    }
    case ND_FOR: {
      Node *typed_node = new_typed_node(new_type(TYPE_NULL), node);
      if (node->init) {
        Node *init = add_type_to_node(function, node->init);
        typed_node->init = init;
      }
      if (node->cond) {
        Node *cond = add_type_to_node(function, node->cond);
        typed_node->cond = cond;
      }
      Node *then = add_type_to_node(function, node->then);
      typed_node->then = then;
      if (node->inc) {
        Node *inc = add_type_to_node(function, node->inc);
        typed_node->inc = inc;
      }
      return typed_node;
    }
    case ND_FUNCALL: {
      // 引数の型の評価を行う
      for (Node *arg = node->expr; arg; arg = arg->next)
        arg->body = add_type_to_node(function, arg->body);
      // 関数名から関数の宣言を探す
      FuncDeclaration *declaration = find_declaration_by_name(node->func_name);
      if (!declaration)
        error("add_type_to_node() : 関数 %s() が宣言されていません", node->func_name);
      return new_typed_node(declaration->ret_type, node);
    }
    case ND_ADDR: {
      Node *lhs = add_type_to_node(function, node->lhs);
      Type *ty = new_type(TYPE_PTR);
      ty->ptr_to = lhs->type;
      return new_typed_binary(new_typed_node(ty, node), lhs, NULL);
    }
    case ND_DEREF: {
      Node *lhs = add_type_to_node(function, node->lhs);
      return new_typed_binary(new_typed_node(lhs->type->ptr_to, node), lhs, NULL);
    }
    case ND_SIZEOF: {
      Node *lhs = add_type_to_node(function, node->lhs);
      Node *size_node = new_size_node(lhs->type);
      // sizeof の lhs に文字列リテラルが直接書かれた場合
      if (lhs->type->kind == TYPE_PTR)
        if (lhs->lhs && lhs->lhs->kind == ND_STR)
          size_node->val = SIZE_CHAR * (find_str_by_id(lhs->lhs->str_id)->len + 1); //　文字列+\0の文字数
      return size_node;
    }
    default:
      error("add_type_to_node() : nodeに型を付与することができませんでした");
  }
}

void semantic_analysis(Node *node) {
  switch (node->kind) {
    case ND_NUM:
    case ND_STR:
    case ND_LVARDEF:
    case ND_LVAR:
    case ND_GVAR:
    case ND_SIZEOF:
      return;
    case ND_STMT:
    case ND_EXPR:
      for (Node *n = node; n; n = n->next)
        semantic_analysis(n->body);
      return;
    case ND_COMMA:
      semantic_analysis(node->lhs);
      semantic_analysis(node->rhs);
      // 左辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(node->lhs))
        node->lhs = cast_array_to_pointer(node->lhs);
      // 右辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(node->rhs))
        node->rhs = cast_array_to_pointer(node->rhs);
      // node自体の型は右辺の型に合わせる
      node->type = node->rhs->type;
      return;
    case ND_ADD:
      semantic_analysis(node->lhs);
      semantic_analysis(node->rhs);
      // 左辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(node->lhs))
        node->lhs = cast_array_to_pointer(node->lhs);
      // 右辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(node->rhs))
        node->rhs = cast_array_to_pointer(node->rhs);
      // ポインタ同士の加算は禁止されているのでエラーにする
      if (is_ptr_type(node->lhs->type) && is_ptr_type(node->rhs->type))
        error("semantic_analysis() : ポインタ同士を加算することはできません");
      // 左辺がint型、右辺がポインタの場合は両辺を入れ替え、node自体の型も付け直す
      if (node->lhs->type->kind == TYPE_INT && node->rhs->type->kind == TYPE_PTR) {
        Node *original_lhs = node->lhs;
        node->lhs = node->rhs;
        node->rhs = original_lhs;
        node->type = node->lhs->type;
      }
      // 左辺がポインタ型、右辺がint型の場合
      if (is_ptr_type(node->lhs->type) && node->rhs->type->kind == TYPE_INT) {
        // node自体の型は左辺の型に合わせる
        node->type = node->lhs->type;
        Node *size_node = new_size_node(node->lhs->type->ptr_to);
        node->rhs = new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_MUL)), size_node, node->rhs);
        return;
      }
      // 両辺が算術型の場合
      else if (is_arithmeric_type(node->lhs->type) && is_arithmeric_type(node->rhs->type)) {
        node->type = new_type(TYPE_INT); // node自体の型はint型にする
        return;
      }
      else
        error("semantic_analysis() : 不正な加算を行うことはできません");
    case ND_SUB:
      semantic_analysis(node->lhs);
      semantic_analysis(node->rhs);
      // 左辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(node->lhs))
        node->lhs = cast_array_to_pointer(node->lhs);
      // 右辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(node->rhs))
        node->rhs = cast_array_to_pointer(node->rhs);
      // 左辺がint型、右辺がポインタ型の減算は禁止されているのでエラーにする
      if (node->lhs->type->kind == TYPE_INT && is_ptr_type(node->rhs->type))
        error("semantic_analysis() : 左辺がint型,右辺がポインタ型の減算を行うことはできません");
      // node自体の型は左辺の型に合わせる
      node->type = node->lhs->type;
      // 左辺がポインタ型、右辺がint型の場合
      if (is_ptr_type(node->lhs->type) && node->rhs->type->kind == TYPE_INT) {
        Node *size_node = new_size_node(node->lhs->type->ptr_to);
        node->rhs = new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_MUL)), size_node, node->rhs);
        return;
      }
      // 両辺がポインタ型の場合
      else if (is_ptr_type(node->lhs->type) && is_ptr_type(node->rhs->type)) {
        if (node->lhs->type->ptr_to->kind != node->rhs->type->ptr_to->kind)
          error("semantic_analysis() : 異なる型へのポインタ同士で減算を行うことはできません");
        Node *size = new_size_node(node->lhs->type->ptr_to);
        Node *diff_addr = new_typed_binary(new_typed_node(node->lhs->type, new_node(ND_SUB)), node->lhs, node->rhs);
        *node = *new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_DIV)), diff_addr, size);
        return;
      }
      // 両辺が算術型の場合
      else if (is_arithmeric_type(node->lhs->type) && is_arithmeric_type(node->rhs->type)) {
        node->type = new_type(TYPE_INT); // node自体の型はint型にする
        return;
      }
      else
        error("semantic_analysis() : 不正な減算を行うことはできません");
    case ND_MUL:
    case ND_DIV:
      semantic_analysis(node->lhs);
      semantic_analysis(node->rhs);
      // 左辺または右辺にポインタ型が来た場合はエラーにする
      if (is_ptr_type(node->lhs->type) || is_ptr_type(node->rhs->type))
        error("semantic_analysis() : ポインタに対し乗法または除法を行うことはできません");
      // nodeそのものはint型であるはずなので、もしそうでなければエラーにする
      if (node->type->kind == TYPE_INT)
        return;
      else
        error("semantic_analysis() : 不正な乗算または除算を行うことはできません");
    case ND_MOD:
      semantic_analysis(node->lhs);
      semantic_analysis(node->rhs);
      if (node->lhs->type->kind != TYPE_INT || node->rhs->type->kind != TYPE_INT)
        error("semantic_analysis() : 剰余演算子の左辺または右辺がint型ではありません");
      // nodeそのものはint型であるはずなので、もしそうでなければエラーにする
      if (node->type->kind == TYPE_INT)
        return;
      else
        error("semantic_analysis() : 不正な除算の剰余を計算することはできません");
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
      semantic_analysis(node->lhs);
      semantic_analysis(node->rhs);
      // 左辺が配列の場合はポインタにキャストする
      if (is_array_type_node(node->lhs))
        node->lhs = cast_array_to_pointer(node->lhs);
      // 右辺が配列の場合はポインタにキャストする
      if (is_array_type_node(node->rhs))
        node->rhs = cast_array_to_pointer(node->rhs);
      // 両辺の型が異なる場合はエラーにする
      if (node->lhs->type->kind != node->rhs->type->kind)
        error("異なる型に対して比較を行うことはできません");
      // nodeそのものはint型であるはずなので、もしそうでなければエラーにする
      if (node->type->kind == TYPE_INT)
        return;
      else
        error("semantic_analysis() : 比較演算子を適用できませんでした");
    case ND_NOT:
      semantic_analysis(node->lhs);
      // 左辺が配列の場合はポインタにキャストする
      if (is_array_type_node(node->lhs))
        node->lhs = cast_array_to_pointer(node->lhs);
      // nodeそのものはint型であるはずなので、もしそうでなければエラーにする
      if (node->type->kind == TYPE_INT)
        return;
      else
        error("semantic_analysis() : 否定演算子を適用できませんでした");
    case ND_AND:
    case ND_OR:
      semantic_analysis(node->lhs);
      semantic_analysis(node->rhs);
      // 左辺が配列の場合はポインタにキャストする
      if (is_array_type_node(node->lhs))
        node->lhs = cast_array_to_pointer(node->lhs);
      // 右辺が配列の場合はポインタにキャストする
      if (is_array_type_node(node->rhs))
        node->rhs = cast_array_to_pointer(node->rhs);
      // nodeそのものはint型であるはずなので、もしそうでなければエラーにする
      if (node->type->kind == TYPE_INT)
        return;
      else
        error("semantic_analysis() : AND演算子またはOR演算子を適用できませんでした");
    case ND_COND:
      semantic_analysis(node->cond);
      semantic_analysis(node->then);
      semantic_analysis(node->els);
      // 各nodeに直接配列が書かれた場合はポインタにキャストする
      if (is_array_type_node(node->cond))
        node->cond = cast_array_to_pointer(node->cond);
      if (is_array_type_node(node->then))
        node->then = cast_array_to_pointer(node->then);
      if (is_array_type_node(node->els))
        node->els = cast_array_to_pointer(node->els);
      // node自体の型はthenかelsの大きい方に合わせておく
      node->type = new_type(larger_arithmetic_type(node->then->type, node->els->type));
      return;
    case ND_ASSIGN:
      semantic_analysis(node->lhs);
      semantic_analysis(node->rhs);
      // 左辺に直接配列が来た場合はエラーにする
      if (is_array_type_node(node->lhs))
        error("semantic_analysis() : 配列に対して直接式の値を代入することはできません1");
      // 右辺が配列型の場合は先頭要素へのポインタにキャストする
      if (is_array_type_node(node->rhs))
        node->rhs = cast_array_to_pointer(node->rhs);
      // node自体の型は左辺の型に合わせる
      node->type = node->lhs->type;
      // 両辺が算術型の場合
      if (is_arithmeric_type(node->lhs->type) && is_arithmeric_type(node->rhs->type)) {
        // 左辺の型のサイズが右辺の型のサイズより大きい場合、右辺の型を左辺の型に合わせる
        // 本来は左辺の型で、右辺の型を全て表現可能かを調べる必要があるが、現時点ではsigned型しかないので大きさを比較すればよい
        if (type_size(node->lhs->type) >= type_size(node->rhs->type)) {
          node->rhs->type = node->lhs->type;
          return;
        }
        // 左辺がchar型、右辺がint型の場合
        if (node->lhs->type->kind == TYPE_CHAR && node->rhs->type->kind == TYPE_INT) {
          node->rhs->type = node->lhs->type;
          return;
        }
      }
      // 両辺で型が異なる場合はエラーにする
      if (node->lhs->type->kind != node->rhs->type->kind)
        error("semantic_analysis() : 代入式の両辺の型が異なります");
      return;
    case ND_RETURN:
      // return <式>; の場合
      if (node->lhs) {
        semantic_analysis(node->lhs);
        // 左辺が配列の場合はポインタにキャストする
        if (is_array_type_node(node->lhs))
          node->lhs = cast_array_to_pointer(node->lhs);
        // node自体の型は左辺の型に合わせる
        node->type = node->lhs->type;
        return;
      }
      // return; の場合はnode自体の型はNULLのはず
      else if (node->type->kind == TYPE_NULL)
        return;
      else
        error("semantic_analysis() : return文の意味解析を行うことができませんでした");
    case ND_IF:
      semantic_analysis(node->cond);
      semantic_analysis(node->then);
      if (node->els)
        semantic_analysis(node->els);
      // node自体の型はNULLのはず
      if (node->type->kind == TYPE_NULL)
        return;
      else
        error("semantic_analysis() : if文の意味解析を行うことができませんでした");
    case ND_FOR:
      if (node->init)
        semantic_analysis(node->init);
      if (node->cond)
        semantic_analysis(node->cond);
      // for文の中身の処理
      semantic_analysis(node->then);
      if (node->inc)
        semantic_analysis(node->inc);
      // node自体の型はNULLのはず
      if (node->type->kind == TYPE_NULL)
        return;
      else
        error("semantic_analysis() : if文の意味解析を行うことができませんでした");
    case ND_FUNCALL:
      for (Node *arg = node->expr; arg; arg = arg->next) {
        semantic_analysis(arg->body);
        if (is_array_type_node(arg->body))
          arg->body = cast_array_to_pointer(arg->body);
      }
      // 関数名から関数の宣言を探す
      FuncDeclaration *declaration = find_declaration_by_name(node->func_name);
      if (!declaration)
        error("semantic_analysis() : 関数 %s() が宣言されていません", node->func_name);
      // node自体の型は関数宣言の型と同じであるはず
      if (node->type->kind == declaration->ret_type->kind)
        return;
      else
        error("semantic_analysis(): 関数呼び出しの意味解析を行うことができませんでした");
    case ND_ADDR:
      semantic_analysis(node->lhs);
      // node自体はポインタ型のはずなので、もしそうでなければエラーにする
      if (is_ptr_type(node->type))
        return;
      else
        error("semantic_analysis() : アドレス演算子を適用することができませんでした");
      return;
    case ND_DEREF:
      semantic_analysis(node->lhs);
      // 左辺が配列の場合はポインタにキャストする
      if (is_array_type_node(node->lhs))
        node->lhs = cast_array_to_pointer(node->lhs);
      // 左辺がポインタ型でない場合はエラーにする
      if (!is_ptr_type(node->lhs->type))
        error("semantic_analysis() : ポインタでないものを間接参照することはできません");
      // node自体の型はlhs->type->ptr_toに合わせる
      node->type = node->lhs->type->ptr_to;
      return;
    default:
      error("semantic_analysis() : 意味解析を行うことができませんでした");
  }
}

// parseして得られたASTに型情報を付与する
// 深さ優先探索で下りながら再帰的に呼び出す
void add_type_to_ast(Function *func_list) {
  for (Function *f = func_list; f; f = f->next) {
    f->body = add_type_to_node(f, f->body);
    semantic_analysis(f->body);
  }
}

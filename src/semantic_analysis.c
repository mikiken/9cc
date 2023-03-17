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
  switch (type->kind) {
    case TYPE_INT:
      size->val = SIZE_INT;
      return size;
    case TYPE_CHAR:
      size->val = SIZE_CHAR;
      return size;
    case TYPE_PTR:
      size->val = SIZE_PTR;
      return size;
    case TYPE_ARRAY:
      size->val = type_size(type->ptr_to) * type->array_size;
      return size;
    default:
      error("new_size_node() : 不明な型のため、型のサイズを計算できませんでした");
  }
}

Node *cast_array_to_pointer(Node *array_node) {
  Node *ref_to_array = new_node(ND_ADDR);
  Type *ty_addr = new_type(TYPE_PTR);
  ty_addr->ptr_to = array_node->type->ptr_to; // 配列の型はptr_toに入っている
  ty_addr->array_size = array_node->type->array_size;
  return new_typed_binary(new_typed_node(ty_addr, ref_to_array), array_node, NULL);
}

bool is_arithmeric_type(TypeKind kind) {
  return kind == TYPE_INT || kind == TYPE_CHAR;
}

bool is_array_type_node(Node *node) {
  return (node->kind == ND_LVAR || node->kind == ND_GVAR) && node->type->kind == TYPE_ARRAY;
}

TypeKind larger_arithmetic_type(Type *ty_1, Type *ty_2) {
  if (!is_arithmeric_type(ty_1->kind) || !is_arithmeric_type(ty_2->kind))
    error("算術型ではありません");
  return type_size(ty_1) >= type_size(ty_2) ? ty_1->kind : ty_2->kind;
}

void fix_rhs_type(Node *lhs, Node *rhs) {
  // NOTE: この関数の処理が詰めきれてない気がする
  // 左辺がchar型で右辺がint型の場合は,右辺をchar型に修正する
  if (lhs->type->kind == TYPE_CHAR && rhs->type->kind == TYPE_INT)
    rhs->type->kind = TYPE_CHAR;
}

Node *add_type_to_node(Obj *lvar_list, Node *node) {
  switch (node->kind) {
    case ND_STMT: {
      Node typed_stmt_head;
      Node *typed_stmt = &typed_stmt_head;

      for (Node *n = node; n; n = n->next) {
        typed_stmt = typed_stmt->next = calloc(1, sizeof(Node));
        typed_stmt->body = add_type_to_node(lvar_list, n->body);
      }
      return typed_stmt_head.next;
    }
    case ND_NUM: {
      return new_typed_node(new_type(TYPE_INT), node);
    }
    case ND_STR: {
      Node *ref_to_str = new_typed_node(new_type(TYPE_PTR), new_node(ND_ADDR));
      ref_to_str->type->ptr_to = new_type(TYPE_CHAR);
      ref_to_str->lhs = new_typed_node(new_type(TYPE_CHAR), node);
      return ref_to_str;
    }
    case ND_LVARDEF:
    case ND_LVAR: {
      return new_typed_node(find_lvar_by_offset(lvar_list, node->offset)->type, node);
    }
    case ND_GVAR: {
      return new_typed_node(find_gvar_by_name(node->gvar_name)->type, node);
    }
    case ND_ASSIGN: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      // 右辺が変数型の場合は先頭要素へのポインタにキャストする
      if (is_array_type_node(rhs))
        rhs = cast_array_to_pointer(rhs);
      fix_rhs_type(lhs, rhs);
      // 両辺で型が異なる場合はエラーにする
      if (lhs->type->kind != rhs->type->kind) {
        error("異なる型の変数を代入することはできません");
      }
      return new_typed_binary(new_typed_node(new_type(lhs->type->kind), node), lhs, rhs);
    }
    case ND_DEREF: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      if (is_array_type_node(lhs)) {
        lhs = cast_array_to_pointer(lhs);
      }
      if (lhs->type->kind != TYPE_PTR) {
        error("ポインタでないものを間接参照することはできません");
      }
      return new_typed_binary(new_typed_node(lhs->type->ptr_to, node), lhs, NULL);
    }
    case ND_ADDR: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Type *ty = new_type(TYPE_PTR);
      ty->ptr_to = lhs->type;
      return new_typed_binary(new_typed_node(ty, node), lhs, NULL);
    }
    case ND_SIZEOF: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *size_node = new_node(ND_NUM);
      if (lhs->type->kind == TYPE_INT) {
        size_node->val = SIZE_INT;
      }
      else if (lhs->type->kind == TYPE_CHAR) {
        size_node->val = SIZE_CHAR;
      }
      else if (lhs->type->kind == TYPE_PTR) {
        if (lhs->lhs && lhs->lhs->kind == ND_STR)
          size_node->val = SIZE_CHAR * (find_str_by_id(lhs->lhs->str_id)->len + 1); //　文字列+\0の文字数
        else
          size_node->val = SIZE_PTR;
      }
      else if ((lhs->kind == ND_LVAR || lhs->kind == ND_GVAR) && lhs->type->kind == TYPE_ARRAY) {
        if (lhs->type->ptr_to->kind == TYPE_INT)
          size_node->val = SIZE_INT * lhs->type->array_size;
        else if (lhs->type->ptr_to->kind == TYPE_CHAR)
          size_node->val = SIZE_CHAR * lhs->type->array_size;
        else
          size_node->val = SIZE_PTR * lhs->type->array_size;
      }
      return new_typed_node(new_type(TYPE_INT), size_node);
    }
    case ND_RETURN: {
      if (node->lhs) {
        Node *lhs = add_type_to_node(lvar_list, node->lhs);
        return new_typed_binary(new_typed_node(new_type(lhs->type->kind), node), lhs, NULL);
      }
      else
        return new_typed_node(new_type(TYPE_NULL), node);
    }
    case ND_IF: {
      Node *typed_node = new_typed_node(new_type(TYPE_NULL), node);
      Node *cond = add_type_to_node(lvar_list, node->cond);
      Node *then = add_type_to_node(lvar_list, node->then);
      if (node->els) {
        Node *els = add_type_to_node(lvar_list, node->els);
        typed_node->els = els;
      }
      typed_node->cond = cond;
      typed_node->then = then;
      return typed_node;
    }
    case ND_FOR: {
      Node *typed_node = new_typed_node(new_type(TYPE_NULL), node);
      if (node->init) {
        Node *init = add_type_to_node(lvar_list, node->init);
        typed_node->init = init;
      }
      if (node->cond) {
        Node *cond = add_type_to_node(lvar_list, node->cond);
        typed_node->cond = cond;
      }
      Node *then = add_type_to_node(lvar_list, node->then);
      typed_node->then = then;
      if (node->inc) {
        Node *inc = add_type_to_node(lvar_list, node->inc);
        typed_node->inc = inc;
      }
      return typed_node;
    }
    case ND_FUNCALL: {
      // 引数
      for (Node *n = node->expr; n; n = n->next) {
        n->body = add_type_to_node(lvar_list, n->body);
        // 引数に直接配列が書かれた場合はポインタにキャストする
        if (is_array_type_node(n->body))
          n->body = cast_array_to_pointer(n->body);
      }
      FuncDeclaration *declaration = find_declaration_by_name(node->func_name);
      if (declaration == NULL) {
        error("%s : 関数が宣言されていません", node->func_name);
      }
      Node *typed_node = new_typed_node(declaration->ret_type, node);
      return typed_node;
    }
    case ND_ADD: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      // 左辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(lhs))
        lhs = cast_array_to_pointer(lhs);
      // 右辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(rhs))
        rhs = cast_array_to_pointer(rhs);
      // ポインタ同士の加算は禁止されているのでエラーにする
      if (lhs->type->kind == TYPE_PTR && rhs->type->kind == TYPE_PTR)
        error("ポインタ同士を加算することはできません");
      // 左辺がint型、右辺がポインタの場合は両辺を入れ替える
      if (lhs->type->kind == TYPE_INT && rhs->type->kind == TYPE_PTR) {
        Node *original_lhs = lhs;
        lhs = rhs;
        rhs = original_lhs;
      }
      // 左辺がポインタ型、右辺がint型の場合
      if (lhs->type->kind == TYPE_PTR && rhs->type->kind == TYPE_INT) {
        Node *size = new_size_node(lhs->type->ptr_to);
        Node *mul_scaling = new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_MUL)), size, rhs);
        return new_typed_binary(new_typed_node(lhs->type, node), lhs, mul_scaling);
      }
      // 両辺が算術型の場合
      else if (is_arithmeric_type(lhs->type->kind) && is_arithmeric_type(rhs->type->kind)) {
        fix_rhs_type(lhs, rhs);
        return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, rhs);
      }
      else {
        error("不正な加算を行うことはできません");
      }
    }
    case ND_SUB: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      // 左辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(lhs))
        lhs = cast_array_to_pointer(lhs);
      // 右辺が配列の場合、ポインタにキャストする
      if (is_array_type_node(rhs))
        rhs = cast_array_to_pointer(rhs);
      // 左辺がint型、右辺がポインタ型の減算は禁止されているのでエラーにする
      if (lhs->type->kind == TYPE_INT && rhs->type->kind == TYPE_PTR)
        error("左辺がint型,右辺がポインタ型の減算を行うことはできません");

      // 左辺がポインタ型、右辺がint型の場合
      if (lhs->type->kind == TYPE_PTR && rhs->type->kind == TYPE_INT) {
        Node *size = new_size_node(lhs->type->ptr_to);
        Node *mul_scaling = new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_MUL)), size, rhs);
        return new_typed_binary(new_typed_node(lhs->type, node), lhs, mul_scaling);
      }
      // 両辺がポインタ型の場合
      else if (lhs->type->kind == TYPE_PTR && rhs->type->kind == TYPE_PTR) {
        if (lhs->type->ptr_to->kind != rhs->type->ptr_to->kind)
          error("異なる型へのポインタ同士で減算を行うことはできません");
        Node *size = new_size_node(lhs->type->ptr_to);
        Node *diff_addr = new_typed_binary(new_typed_node(lhs->type, new_node(ND_SUB)), lhs, rhs);
        return new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_DIV)), diff_addr, size);
      }
      // 両辺が算術型の場合
      else if (is_arithmeric_type(lhs->type->kind) && is_arithmeric_type(rhs->type->kind)) {
        fix_rhs_type(lhs, rhs);
        return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, rhs);
      }
      else {
        error("不正な減算を行うことはできません");
      }
    }
    case ND_MUL:
    case ND_DIV: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      if (lhs->type->kind != rhs->type->kind)
        error("ポインタに対し乗法または除法を行うことはできません");
      return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, rhs);
    }
    case ND_MOD: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      if (lhs->type->kind != TYPE_INT || rhs->type->kind != TYPE_INT)
        error("剰余演算子の左辺または右辺がint型ではありません");
      return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, rhs);
    }
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      // 左辺が配列の場合はポインタにキャストする
      if (is_array_type_node(lhs))
        lhs = cast_array_to_pointer(lhs);
      // 右辺が配列の場合はポインタにキャストする
      if (is_array_type_node(rhs))
        rhs = cast_array_to_pointer(rhs);
      // 両辺の型が異なる場合はエラーにする
      if (lhs->type->kind != rhs->type->kind)
        error("異なる型に対して比較を行うことはできません");
      return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, rhs);
    }
    case ND_NOT: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      // 左辺が配列の場合はポインタにキャストする
      if (is_array_type_node(lhs))
        lhs = cast_array_to_pointer(lhs);
      return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, NULL);
    }
    case ND_AND:
    case ND_OR: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      // 左辺が配列の場合はポインタにキャストする
      if (is_array_type_node(lhs))
        lhs = cast_array_to_pointer(lhs);
      // 右辺が配列の場合はポインタにキャストする
      if (is_array_type_node(rhs))
        rhs = cast_array_to_pointer(rhs);
      return new_typed_binary(new_typed_node(new_type(TYPE_INT), node), lhs, rhs);
    }
    case ND_COND: {
      Node *cond = add_type_to_node(lvar_list, node->cond);
      Node *then = add_type_to_node(lvar_list, node->then);
      Node *els = add_type_to_node(lvar_list, node->els);
      // 各nodeに直接配列が書かれた場合はポインタにキャストする
      if (is_array_type_node(cond))
        cond = cast_array_to_pointer(cond);
      if (is_array_type_node(then))
        then = cast_array_to_pointer(then);
      if (is_array_type_node(els))
        els = cast_array_to_pointer(els);
      Node *typed_node = new_typed_node(new_type(larger_arithmetic_type(then->type, els->type)), node);
      typed_node->cond = cond;
      typed_node->then = then;
      typed_node->els = els;
      return typed_node;
    }
    case ND_COMMA: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      // 左辺が配列の場合はポインタにキャストする
      if (is_array_type_node(lhs))
        lhs = cast_array_to_pointer(lhs);
      // 右辺が配列の場合はポインタにキャストする
      if (is_array_type_node(rhs))
        rhs = cast_array_to_pointer(rhs);
      return new_typed_binary(new_typed_node(rhs->type, node), lhs, rhs);
    }
    default:
      error("nodeに型を付与することができませんでした");
  }
}

// parseして得られたASTに型情報を付与する
// 深さ優先探索で下りながら再帰的に呼び出す
void add_type_to_ast(Function *func_list) {
  for (Function *f = func_list; f; f = f->next) {
    f->body = add_type_to_node(f->lvar_list, f->body); // 型付きASTを構築
  }
}

#include "9cc.h"

FuncDeclaration *find_declaration_by_name(char *name) {
  for (FuncDeclaration *declaration = func_declaration_list; declaration; declaration = declaration->next) {
    if (!name) {
      error("nameがNULLポインタです");
    }
    if (!declaration->name) {
      error("declaration->nameがNULLポインタです");
    }
    if (!strcmp(declaration->name, name)) { //名前が一致している場合
      return declaration;
    }
  }
  return NULL;
}

Lvar *find_lvar_by_offset(Lvar *lvar_list, int offset) {
  for (Lvar *lvar = lvar_list; lvar; lvar = lvar->next) {
    if (lvar->offset == offset) {
      return lvar;
    }
  }
  return NULL;
}

// 変数の型を返す関数
Type *return_lvar_type(Lvar *lvar) {
  return lvar->type;
}

Type *new_type(TypeKind kind) {
  Type *type = calloc(1, sizeof(Type));
  type->kind = kind;
}
// 型なしnodeに型を追加した新たなnodeを返す
Node *new_typed_node(Type *type, Node *node) {
  Node *node_typed = calloc(1, sizeof(Node));
  *node_typed = *node; // nodeをコピー
  node_typed->type = type; // 型情報を付加
  return node_typed;
}

Node *new_typed_binary(Node *node_typed, Node *lhs_typed, Node *rhs_typed) {
  node_typed->lhs = lhs_typed;
  node_typed->rhs = rhs_typed;
  return node_typed;
}

Node *cast_array_to_pointer(Node *array_node) {
  Node *ref_to_array = new_node(ND_ADDR);
  Type *ty_addr = calloc(1, sizeof(Type));
  ty_addr->kind = TYPE_PTR;
  ty_addr->ptr_to = array_node->type->ptr_to; // 配列の型はptr_toに入っている
  ty_addr->array_size = array_node->type->array_size;
  return new_typed_binary(new_typed_node(ty_addr, ref_to_array), array_node, NULL);
}

Node *add_type_to_node(Lvar *lvar_list, Node *node) {
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
    case ND_NUM: { // ひとまずint型にする(もしポインタとの和の場合は case ND_ADD: など、後で修正される)
      Type *ty = calloc(1, sizeof(Type));
      ty->kind = TYPE_INT;
      return new_typed_node(ty, node);
    }
    case ND_LVARDEF:
    case ND_LVAR: {
      Lvar *lvar = find_lvar_by_offset(lvar_list, node->offset);
      Type *ty = calloc(1, sizeof(Type));
      if (return_lvar_type(lvar)->kind == TYPE_INT) {
        ty->kind = TYPE_INT;
        return new_typed_node(ty, node);
      } else if (return_lvar_type(lvar)->kind == TYPE_PTR) {
        ty->kind = TYPE_PTR;
        ty->ptr_to = lvar->type->ptr_to;
        return new_typed_node(ty, node);
      } else if (return_lvar_type(lvar)->kind == TYPE_ARRAY) {
        ty->kind = TYPE_ARRAY;
        ty->ptr_to = lvar->type->ptr_to;
        ty->array_size = lvar->type->array_size;
        return new_typed_node(ty, node);
      }
    }
    case ND_ASSIGN: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      if (rhs->kind == ND_LVAR && rhs->type->kind == TYPE_ARRAY) {
        rhs = cast_array_to_pointer(rhs);
      }
      if (lhs->type->kind != rhs->type->kind) {
        error("異なる型の変数を代入することはできません");
      }
      Type *ty = calloc(1, sizeof(Type));
      ty->kind = lhs->type->kind;
      return new_typed_binary(new_typed_node(ty, node), lhs, rhs);
    }
    case ND_DEREF: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      if (lhs->kind == ND_LVAR && lhs->type->kind == TYPE_ARRAY) {
        lhs = cast_array_to_pointer(lhs);
      }
      if (lhs->type->kind != TYPE_PTR) {
        error("ポインタでないものを間接参照することはできません");
      }
      return new_typed_binary(new_typed_node(lhs->type->ptr_to, node), lhs, NULL);
    }
    case ND_ADDR: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Type *ty = calloc(1, sizeof(Type));
      ty->kind = TYPE_PTR;
      ty->ptr_to = lhs->type;
      return new_typed_binary(new_typed_node(ty, node), lhs, NULL);
    }
    case ND_SIZEOF: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Type *ty = calloc(1, sizeof(Type));
      ty->kind = TYPE_INT;
      Node *size_node = new_node(ND_NUM);
      if (lhs->type->kind == TYPE_INT) {
        size_node->val = SIZE_INT;
      } else if (lhs->type->kind == TYPE_PTR) {
        size_node->val = SIZE_PTR;
      } else if (lhs->kind == ND_LVAR && lhs->type->kind == TYPE_ARRAY) {
        if (lhs->type->ptr_to->kind == TYPE_INT)
          size_node->val = SIZE_INT * lhs->type->array_size;
        else
          size_node->val = SIZE_PTR * lhs->type->array_size;
      }
      return new_typed_node(ty, size_node);
    }
    case ND_RETURN: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Type *ty = calloc(1, sizeof(Type));
      ty->kind =lhs->type->kind;
      return new_typed_binary(new_typed_node(ty, node), lhs, NULL);
    }
    case ND_IF: {
      Type *ty = calloc(1, sizeof(Type));
      ty->kind = TYPE_NULL;
      Node *typed_node = new_typed_node(ty, node);     
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
      Type *ty = calloc(1, sizeof(Type));
      ty->kind = TYPE_NULL;
      Node *typed_node = new_typed_node(ty, node);
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
      }
      FuncDeclaration *declaration = find_declaration_by_name(node->func_name);
      if (declaration == NULL) {
        error("関数が宣言されていません");
      }
      Node *typed_node = new_typed_node(declaration->ret_type, node);
      return typed_node;
    }
    case ND_ADD: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      // 左辺が配列の場合、ポインタにキャストする
      if (lhs->kind == ND_LVAR && lhs->type->kind == TYPE_ARRAY)
        lhs = cast_array_to_pointer(lhs);
      // 右辺が配列の場合、ポインタにキャストする
      if (rhs->kind == ND_LVAR && rhs->type->kind == TYPE_ARRAY)
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
        Node *size = new_typed_node(new_type(TYPE_INT), new_node(ND_NUM));
        if (lhs->type->ptr_to->kind == TYPE_INT) {
          size->val = SIZE_INT;
        } else {
          size->val = SIZE_PTR;
        }
        Node *mul_scaling = new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_MUL)), size, rhs);
        return new_typed_binary(new_typed_node(lhs->type, node), lhs, mul_scaling);
      }
      // 両辺がint型の場合
      else if (lhs->type->kind == TYPE_INT && rhs->type->kind == TYPE_INT) {
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
      if (lhs->kind == ND_LVAR && lhs->type->kind == TYPE_ARRAY)
        lhs = cast_array_to_pointer(lhs);
      // 右辺が配列の場合、ポインタにキャストする
      if (rhs->kind == ND_LVAR && rhs->type->kind == TYPE_ARRAY)
        rhs = cast_array_to_pointer(rhs);
      // 左辺がint型、右辺がポインタ型の減算は禁止されているのでエラーにする
      if (lhs->type->kind == TYPE_INT && rhs->type->kind == TYPE_PTR)
        error("左辺がint型,右辺がポインタ型の減算を行うことはできません");
      
      // 左辺がポインタ型、右辺がint型の場合
      if (lhs->type->kind == TYPE_PTR && rhs->type->kind == TYPE_INT) {
        Node *size = new_typed_node(new_type(TYPE_INT), new_node(ND_NUM));
        if (lhs->type->ptr_to->kind == TYPE_INT) {
          size->val = SIZE_INT;
        } else {
          size->val = SIZE_PTR;
        }
        Node *mul_scaling = new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_MUL)), size, rhs);
        return new_typed_binary(new_typed_node(lhs->type, node), lhs, mul_scaling);
      }
      // 両辺がポインタ型の場合
      else if (lhs->type->kind == TYPE_PTR && rhs->type->kind == TYPE_PTR) {
        if (lhs->type->ptr_to->kind != rhs->type->ptr_to->kind)
          error("異なる型へのポインタ同士で減算を行うことはできません");
        Node *size = new_typed_node(new_type(TYPE_INT), new_node(ND_NUM));
        if (lhs->type->ptr_to->kind == TYPE_INT) {
          size->val = SIZE_INT;
        } else {
          size->val = SIZE_PTR;
        }
        Node *diff_addr = new_typed_binary(new_typed_node(lhs->type, new_node(ND_SUB)), lhs, rhs);
        return new_typed_binary(new_typed_node(new_type(TYPE_INT), new_node(ND_DIV)), diff_addr, size);
      }
      // 両辺がint型の場合
      else if (lhs->type->kind == TYPE_INT && rhs->type->kind == TYPE_INT) {
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
      if (lhs->type->kind != rhs->type->kind) {
        error("ポインタに対し乗法または除法を行うことはできません");
      }
      // そうでなければint同士の和差のはず
      Type *ty = calloc(1, sizeof(Type));
      ty->kind = TYPE_INT;
      return new_typed_binary(new_typed_node(ty, node), lhs, rhs);
    }
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      if (lhs->type->kind != rhs->type->kind) {
        error("異なる型に対して比較を行うことはできません");
      }
      Type *ty = calloc(1, sizeof(Type));
      ty->kind =lhs->type->kind;
      return new_typed_binary(new_typed_node(ty, node), lhs, rhs);
    }
    default:
      error("***************************\n"
            "* INTERNAL COMPILER ERROR *\n"
            "***************************\n");
  }
}

// parseして得られたASTに型情報を付与する
// 深さ優先探索で下りながら再帰的に呼び出す
void add_type_to_ast(Function *func_list) {
  for (Function *f = func_list; f; f = f->next) {
    f->body = add_type_to_node(f->lvar_list, f->body); // 型付きASTを構築
  }
}


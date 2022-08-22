#include "9cc.h"
#define SIZE_INT 4
#define SIZE_PTR 8

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
      if (lhs->type->kind != rhs->type->kind) {
        error("異なる型の変数を代入することはできません");
      }
      Type *ty = calloc(1, sizeof(Type));
      ty->kind = lhs->type->kind;
      return new_typed_binary(new_typed_node(ty, node), lhs, rhs);
    }
    case ND_DEREF: {
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
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
      } else { // TYPE_PTR
        size_node->val = SIZE_PTR;
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
        add_type_to_node(lvar_list, n->body);
      }
      FuncDeclaration *declaration = find_declaration_by_name(node->func_name);
      if (declaration == NULL) {
        error("関数が宣言されていません");
      }
      Node *typed_node = new_typed_node(declaration->ret_type, node);
      return typed_node;
    }
    case ND_ADD:
    case ND_SUB: { // 現状では左辺と右辺で型が違う場合はTYPR_PTRに合わせればいいはず
      Type *ty = calloc(1, sizeof(Type));
      Node *lhs = add_type_to_node(lvar_list, node->lhs);
      Node *rhs = add_type_to_node(lvar_list, node->rhs);
      if (lhs->type->kind == TYPE_PTR && rhs->type->kind == TYPE_INT) {
        Type *size_ty = calloc(1, sizeof(Type));
        size_ty->kind = TYPE_INT;
        Node *size = calloc(1, sizeof(Node));
        size->kind = ND_NUM;
        size->type = size_ty;
        if (lhs->type->ptr_to->kind == TYPE_INT) {
          size->val = SIZE_INT; // int
        } else {
          size->val = SIZE_PTR;
        }
        Type *mul_ty = calloc(1, sizeof(Type));
        Node *multiply_offset = calloc(1, sizeof(Node));
        multiply_offset->kind = ND_MUL;
        mul_ty->kind = TYPE_INT;
        multiply_offset->type = mul_ty;
        multiply_offset->lhs = size;
        multiply_offset->rhs = rhs;
        node->rhs = multiply_offset;
        node->lhs = lhs;
        Node *typed_node = new_typed_node(lhs->type, node);
        // nodeの両辺のnodeを型ありnodeに付け替える
        return typed_node;
      } else if (lhs->type->kind == TYPE_INT && rhs->type->kind == TYPE_INT) {
        // そうでなければint同士の和差のはず
        ty->kind = TYPE_INT;
        return new_typed_binary(new_typed_node(ty, node), lhs, rhs);
      } else {
        error("ポインタの加減算のうち未実装のものです");
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
void make_typed_ast() {
  for (Function *f = func_head.next; f; f = f->next) {
    f->body = add_type_to_node(f->locals, f->body); // 型付きASTを構築
  }
}


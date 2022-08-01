# 9cc
C言語(のサブセット)コンパイラ

## Implemented
- `+` `-` `*` `/` operators
- `<` `<=` `>` `>=` `==` `!=` operators
- local variables
- `return` statement
- `if` `else` statement
- `while` `for` statement
- `{…}` compound statements (blocks)
- function call with up to 6 args

## BNF
```
program    = stmt*
stmt       = expr ";"
           | "{" stmt* "}"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while" "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident args? | "(" expr ")"
args       = "(" (expr ("," expr)*)? ")"
```

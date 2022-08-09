# 9cc
C言語(のサブセット)コンパイラ

## Implemented
- `+` `-` `*` `/` operators
- `<` `<=` `>` `>=` `==` `!=` operators
- unary `*` `&` operators
- local variables
- `return` statement
- `if` `else` statement
- `while` `for` statement
- `{…}` compound statements (blocks)
- function call and definition with up to 6 args
- keyword `int`
- pointer type


## BNF
```
program    = func_def*
func_def   = type ident params "{" stmt* "}"
type       = "int" "*"*
params     = "(" (type ident ("," type ident)*)? ")"
stmt       = expr ";"
           | "{" stmt* "}"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while" "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "return" expr ";"
expr       = assign | type ident
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = "+"? primary | "-"? primary | "*" unary | "&" unary
primary    = num | ident args? | "(" expr ")"
args       = "(" (expr ("," expr)*)? ")"
```

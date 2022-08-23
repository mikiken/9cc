# 9cc
C言語(のサブセット)コンパイラ

# build
```
# build
$ git clone https://github.com/mikiken/9cc.git
$ cd 9cc
$ make

# test
$ make test
```

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
- pointer add and sub
- `sizeof` operator
- one-dimensional array

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
expr       = assign | type ident("[" num "]")?
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = "sizeof" unary | "+"? primary | "-"? primary | "*" unary | "&" unary
primary    = num | ident post_fix? | "(" expr ")"
post_fix   = "[" expr "]" | args?
args       = "(" (expr ("," expr)*)? ")"
```

# 9cc
C言語(のサブセット)コンパイラ


# How to build
## build
```
$ git clone https://github.com/mikiken/9cc.git
$ cd 9cc
$ make
```
## test
```
$ make test
```

# Implemented
- `+` `-` `*` `/` `%` operators
- `+=` `-=` `*=` `/=` `%=` operators
- `++` `--` operators
- `<` `<=` `>` `>=` `==` `!=` operators
- `!` `&&` `||` operators
- `?:` operator
- unary `*` `&` operators
- local variables
- global variables
- `return` statement
- `if` `else` statement
- `while` `for` statement
- `{…}` compound statements (blocks)
- function call and definition with up to 6 args
- function declaration without type checking
- `int` `char` `void` type
- string literal
- pointer type
- pointer add and sub
- `sizeof` operator
- one-dimensional array


# BNF
```
program     = (global_var | func_def)*
grobal_var  = type ident("[" num "]")? ";"
func_def    = type ident params "{" stmt* "}"
type        = ("int" | "char" | "void") "*"*
params      = "(" ((type ident ("," type ident)*) | "void")? ")"
stmt        = expr ";"
            | "{" stmt* "}"
            | "if" "(" expr ")" stmt ("else" stmt)?
            | "while" "(" expr ")" stmt
            | "for" "(" expr? ";" expr? ";" expr? ")" stmt
            | "return" expr? ";"
expr        = assign | type ident("[" num "]")?
assign      = conditional | conditional ("=" | "+=" | "-=" | "*=" | "/=" | "%=" ) assign
conditional = logical_or ("?" expr ":" conditional)?
logical_or  = logical_and ("||" logical_and)*
logical_and = equality ("&&" equality)*
equality    = relational ("==" relational | "!=" relational)*
relational  = add ("<" add | "<=" add | ">" add | ">=" add)*
add         = mul ("+" mul | "-" mul)*
mul         = unary ("*" unary | "/" unary | "%" unary)*
unary       = postfix | ("sizeof" | "+" | "-" | "*" | "&" | "++" | "--" | "!") unary
postfix     = primary ("[" expr "]" | "++" | "--")?
primary     = num | ident args? | string | "(" expr ")"
args        = "(" (expr ("," expr)*)? ")"
```

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
- `,` operator
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
- multi-dimensional array

# BNF
```
<program> := (<global_var_declaration> | <func_definition> | <func_declaration>)*

<global_var_declaration> := <declaration> <type_suffix>? ";"

<func_definition> := <declaration> <parameter_list> <block_stmt>

<func_declaration> = <declaration> <parameter_list> ";" # 引数リストのパースは行わず、単純に")"までは読み飛ばす

<declaration> := <declaration_specifier> <identifier>

<declaration_specifier> := <type_specifier> "*"* | "void" "*"?

<type_specifier> := "int" | "char"

<type_suffix> := "[" <integer_constant> "]"

<parameter_list> := "(" (<parameter> | "void")? ")"

<parameter> := <declaration> ("," <declaration>)*

<block_stmt> := "{" (<stmt> | <declaration> ";")* "}"

<stmt> := <expr> ";"
        | <block_stmt>
        | "if" "(" <expr> ")" <stmt> ("else" <stmt>)?
        | "while" "(" <expr> ")" <stmt>
        | "for" "(" <expr>? ";" <expr>? ";" <expr>? ")" <stmt>
        | "return" <expr>? ";"

<expr> := <assign> ("," <expr>)?
       
<assign>      := <conditional> (("=" | "+=" | "-=" | "*=" | "/=" | "%=" ) <assign>)?

<conditional> := <logical_or> ("?" <expr> ":" <conditional>)?

<logical_or>  := <logical_and> ("||" <logical_and>)*

<logical_and> := <equality> ("&&" <equality>)*

<equality>    := <relational> (("==" | "!=") <relational>)*

<relational>  := <add> (("<" | "<=" | ">" | ">=") <add>)*

<add>         := <mul> (("+"|"-") <mul>)*

<mul>         := <unary> (("*" | "/" | "%") <unary>)*

<unary>       := <postfix>
               | ("++" | "--") <unary>
               | ("&" | "*" | "+" | "-" | "!" |) <unary>
               | "sizeof" <unary>

# TODO: "sizeof" "(" <type_name> ")" に対応する 

<postfix>     := <primary> ("[" <expr> "]" | "++" | "--")? # NOTE: 多次元配列は現在未対応

<primary>   := <identifier>
             | <identifier> "(" <arg_list>? ")"
             | <integer_constant>
             | <string_literal>
             | "(" <expr> ")"

<arg_list> := <assign> ("," <assign>)*
```
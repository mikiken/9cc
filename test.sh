#!/bin/bash
cat << EOT >> tmp2.c
#include <stdlib.h>

int foo() { return 5; }
int add2(int a, int b) {return a+b;}
int add6(int a, int b, int c, int d, int e, int f) {return a+b+c+d+e+f;}
int sub2(int a, int b) {return a-b;}
void alloc4(int **p, int a, int b, int c, int d) {
  *p = malloc(sizeof(int) * 5);
  (*p)[0] = a;
  (*p)[1] = b;
  (*p)[2] = c;
  (*p)[3] = d;
  (*p)[4] = 0;
}
void no_interaction_to_ptr(int *p){};
void no_interaction_to_ptr_to_ptr(int **p){};
EOT
cc -c tmp2.c

assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s #9ccを実行し、アセンブリ(.s)を出力
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert_funcall() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s #9ccを実行し、アセンブリ(.s)を出力
  cc -c tmp.s
  cc -o tmp tmp.o tmp2.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 'int main(){return 0;}'
assert 42 'int main(){return 42;}'

assert 21 'int main(){return 5+20-4;}'
assert 41 'int main(){return 12 + 34 - 5 ;}'
assert 148 'int main(){return 100 + 56 - 8 ;}'

assert 47 'int main(){return 5+6*7;}'
assert 15 'int main(){return 5*(9-6);}'
assert 4 'int main(){return (3+5)/2;}'

assert 10 'int main(){return -10+20;}'
assert 10 'int main(){return +(+10);}'
assert 10 'int main(){return -(-10);}'
assert 10 'int main(){return (-(-15 + 25) + 50) / 4;}'

assert 0 'int main(){return 0==1;}'
assert 1 'int main(){return 42==42;}'
assert 1 'int main(){return 0!=1;}'
assert 0 'int main(){return 42!=42;}'

assert 1 'int main(){return 0<1;}'
assert 0 'int main(){return 1<1;}'
assert 0 'int main(){return 2<1;}'
assert 1 'int main(){return 0<=1;}'
assert 1 'int main(){return 1<=1;}'
assert 0 'int main(){return 2<=1;}'

assert 1 'int main(){return 1>0;}'
assert 0 'int main(){return 1>1;}'
assert 0 'int main(){return 1>2;}'
assert 1 'int main(){return 1>=0;}'
assert 1 'int main(){return 1>=1;}'
assert 0 'int main(){return 1>=2;}'

assert 2 'int main(){int a; a=2; return a;}'
assert 6 'int main(){int x; x=4; int y; y=2; return x+y;}'
assert 9 'int main(){int p; int q; int r; p=q=3; r=2; return p+q*r;}'

assert 5 'int main(){int foo; foo=5; return foo;}'
assert 8 'int main(){int bar; bar=2; return bar*3+2;}'
assert 6 'int main(){int _foo123; int bar; _foo123=4; bar=2; return _foo123+bar;}'
assert 4 'int main(){int Foo_123_bar; Foo_123_bar=3; return (-Foo_123_bar+5)*2;}'

assert 1 'int main(){return 1; 2; 3;}'
assert 2 'int main(){1; return 2; 3;}'
assert 3 'int main(){1; 2; return 3;}'
assert 3 'int main(){1; 2; return 3; return 4;}'
assert 5 'int main(){int return123; return123 = 3; return return123 + 2;}'

assert 5 'int main(){if(0) 3; return 5;}'
assert 8 'int main(){if (1) return 8;}'
assert 2 'int main(){if(1) return 2; return 7;}'
assert 6 'int main(){if (2+3==5) return 3*2;}'
assert 3 'int main(){int a; a=-1; if(a>0) return 5; return 3;}'
assert 2 'int main(){if(0) return 3; else return 2;}'
assert 3 'int main(){if(1) return 3; else return 2;}'
assert 4 'int main(){if(0) return 3; else 2; return 4;}'
assert 4 'int main(){int a; a=2*3; if(a<5) return 3; else if(a==6) return 4;}'
assert 2 'int main(){int a; a=2*3; if(a<5) return 3; else if(a==7) return 4; return 2;}'

assert 2 'int main(){while(0) return 1; return 2;}'
assert 5 'int main(){int x; x=0; while(x<5) x=x+1; return x;}'

assert 5 'int main(){int x; int i; x=0; for(i=0;i<5;i=i+1) x=x+1; return x;}'
assert 3 'int main(){for (;;) return 3;}'

assert 2 'int main(){ return 2; }'
assert 8 'int main(){int a; int b; a=2; b = 6; return a+b;}'
assert 3 'int main(){ {1; {2;} return 3;} }'
assert 20 'int main(){int a; int i; a=0; for(i=0; i<10; i=i+1){a=a+2;} return a;}'

assert 6 'int main(){int a; int i; a=0; for(i=0; i<10; i=i+1){a=a+2; if(a==6) return a;}}'
assert 40 'int main(){int a; int b; int i; int j; a=0; b=0; for(i=0; i<10; i=i+1){a=a+1; if(i==9){a=a*2;}} for(j=0; j<10; j=j+1){b=b+1; if(j==9){b=b*2;}} return a+b;}'
assert 3 'int main(){int a; a=4; if(a==4){a=a+2;if(a==6) {a=a-3; return a;}}}'
assert 25 'int main(){int sum; int i; int j; sum=0; for(i=1;i<=5;i=i+1){for(j=1;j<=5;j=j+1){sum=sum+1;}} return sum;}'
assert 5 'int main(){int a; a=5; if(a>=0){if(a==5){return a;}}}'
assert 10 'int main(){int a; int b; a=3; {b=7; if(a==3) return a+b;}}'

assert 3 'int main(){int a; a=3; if(a==1) return 1; if(a==2) return 2; if(a==3) return 3;}'

assert_funcall 5 'int foo(); int main(){return foo();}'
assert_funcall 8 'int foo(); int main(){{return foo() + 3;}}'

assert_funcall 7 'int add2(); int main(){return add2(2, 5);}'
assert_funcall 3 'int sub2(); int main(){return sub2(10, 7);}'
assert_funcall 21 'int add6(); int main(){return add6(1,2,3,4,5,6);}'
assert_funcall 21 'int add2(); int add6(); int main(){return add6(1,2,3,4,5,add2(2,4));}'
assert_funcall 66 'int add2(); int add6(); int main(){return add6(add6(1,2,3,add2(1,3),5,6),7,8,9,10,11);}'

assert 5 'int ret3(){return 3;} int main(){return ret3()+2;}'
assert 11 'int ret6(){int a; int b; a=3; b=2; return a*b;} int main(){int a; a=5; return ret6()+a;}'

assert 24 'int fact(int n){if(n==1) return 1; return n * fact(n-1);} int main(){return fact(4);}'
assert 55 'int fib(int n) {if (n==1) {return 1;}if (n==2) {return 1;} return fib(n-1) + fib(n-2);} int main() {return fib(10);}'
assert 15 'int combi(int n, int r){if(r==0) return 1; else if(n==r) return 1; else return combi(n-1,r-1) + combi(n-1,r);} int main(){return combi(6,2);}'

# pointer type
assert 5 'int main(){int x; int *y; x=3; y=&x; return *y+2;}'
assert 3 'int main(){int x; x=3; return *&x; }'
assert 3 'int main(){int x; int *y; int **z; x=3; y=&x; z=&y; return **z; }'
assert 5 'int main(){int x; int *y; x=3; y=&x; *y=5; return x; }'
# ↓未定義動作なのでコメントアウト
#assert 5 'int main(){int x; int y; x=3; y=5; return *(&x-2); }'
#assert 3 'int main(){int x; int y; x=3; y=5; return *(&y+2); }'
#assert 7 'int main(){int x; int y; x=3; y=5; *(&x-2)=7; return y; }'
#assert 7 'int main(){int x; int y; x=3; y=5; *(&y+2)=7; return x; }'

# function declaration
assert_funcall 0 'int no_interaction_to_ptr(); int main() {int p; no_interaction_to_ptr(&p); return 0;}'
assert_funcall 0 'int no_interaction_to_ptr_to_ptr(); int main() {int *p; no_interaction_to_ptr_to_ptr(&p); return 0;}'
assert 6 'int main() {int a; int *b; int **c; int ***d; a=6; b=&a; c=&b; d=&c; return ***d;}'

assert_funcall 4 'int alloc4(); int main(){int *p; alloc4(&p, 1, 2, 4, 8); int *q; q=p+2; return *q;}'
assert_funcall 2 'int alloc4(); int main(){int *p; alloc4(&p, 1, 2, 4, 8); int *q; q=p+2; q=q-1; return *q;}'

# sizeof operator
assert 4 'int main(){int x; return sizeof(x);}'
assert 4 'int main(){int x; return sizeof(x+2);}'
assert 4 'int main(){int x; return sizeof(x=2);}'
assert 4 'int main(){int x; return sizeof x;}'
assert 8 'int main(){int *x; return sizeof(x);}'
assert 8 'int main(){int *x; return sizeof x;}'
assert 8 'int main(){int x; return sizeof(&x);}'

echo OK
rm tmp tmp.o tmp2.c tmp2.o
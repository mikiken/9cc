#!/bin/bash
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

assert 0 '0;'
assert 42 '42;'

assert 21 '5+20-4;'
assert 41 ' 12 + 34 - 5 ;'
assert 148 ' 100 + 56 - 8 ;'

assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'

assert 10 '-10+20;'
assert 10 '+(+10);'
assert 10 '-(-10);'
assert 10 '+ +10;'
assert 10 '- -10;'
assert 10 '(-(-15 + 25) + 50) / 4;'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 2 'a=2; a;'
assert 6 'x=4; y=2; x+y;'
assert 9 'p=q=3; r=2; p+q*r;'

assert 5 'foo=5; foo;'
assert 8 'bar=2; bar*3+2;'
assert 6 '_foo123=4; bar=2; _foo123+bar;'
assert 4 'Foo_123_bar=3; (-Foo_123_bar+5)*2;'

assert 1 'return 1; 2; 3;'
assert 2 '1; return 2; 3;'
assert 3 '1; 2; return 3;'
assert 3 '1; 2; return 3; return 4;'
assert 5 'return123 = 3; return return123 + 2;'

assert 5 'if(0) 3; 5;'
assert 8 'if (1) 8;'
assert 2 'if(1) return 2; return 7;'
assert 6 'if (2+3==5) 3*2;'
assert 3 'a=-1; if(a>0) return 5; return 3;'
assert 2 'if(0) 3; else 2;'
assert 3 'if(1) 3; else 2;'
assert 4 'if(0) 3; else 2; 4;'
assert 4 'a=2*3; if(a<5) return 3; else if(a==6) return 4;'
assert 2 'a=2*3; if(a<5) return 3; else if(a==7) return 4; return 2;'

assert 2 'while(0) return 1; return 2;'
assert 5 'x=0; while(x<5) x=x+1; return x;'

assert 5 'x=0; for(i=0;i<5;i=i+1) x=x+1; return x;'
assert 3 'for (;;) return 3;'

assert 2 '{ return 2; }'
assert 8 '{a=2; b = 6; return a+b;}'
assert 3 '{ {1; {2;} return 3;} }'
assert 20 'a=0; for(i=0; i<10; i=i+1){a=a+2;} return a;'

echo OK

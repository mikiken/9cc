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

assert 0 'return 0;'
assert 42 'return 42;'

assert 21 'return 5+20-4;'
assert 41 'return 12 + 34 - 5 ;'
assert 148 'return 100 + 56 - 8 ;'

assert 47 'return 5+6*7;'
assert 15 'return 5*(9-6);'
assert 4 'return (3+5)/2;'

assert 10 'return -10+20;'
assert 10 'return +(+10);'
assert 10 'return -(-10);'
assert 10 'return + +10;'
assert 10 'return - -10;'
assert 10 'return (-(-15 + 25) + 50) / 4;'

assert 0 'return 0==1;'
assert 1 'return 42==42;'
assert 1 'return 0!=1;'
assert 0 'return 42!=42;'

assert 1 'return 0<1;'
assert 0 'return 1<1;'
assert 0 'return 2<1;'
assert 1 'return 0<=1;'
assert 1 'return 1<=1;'
assert 0 'return 2<=1;'

assert 1 'return 1>0;'
assert 0 'return 1>1;'
assert 0 'return 1>2;'
assert 1 'return 1>=0;'
assert 1 'return 1>=1;'
assert 0 'return 1>=2;'

assert 2 'a=2; return a;'
assert 6 'x=4; y=2; return x+y;'
assert 9 'p=q=3; r=2; return p+q*r;'

assert 5 'foo=5; return foo;'
assert 8 'bar=2; return bar*3+2;'
assert 6 '_foo123=4; bar=2; return _foo123+bar;'
assert 4 'Foo_123_bar=3; return (-Foo_123_bar+5)*2;'

assert 1 'return 1; 2; 3;'
assert 2 '1; return 2; 3;'
assert 3 '1; 2; return 3;'
assert 3 '1; 2; return 3; return 4;'
assert 5 'return123 = 3; return return123 + 2;'

assert 5 'if(0) 3; return 5;'
assert 8 'if (1) return 8;'
assert 2 'if(1) return 2; return 7;'
assert 6 'if (2+3==5) return 3*2;'
assert 3 'a=-1; if(a>0) return 5; return 3;'
assert 2 'if(0) return 3; else return 2;'
assert 3 'if(1) return 3; else return 2;'
assert 4 'if(0) return 3; else 2; return 4;'
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

assert 6 'a=0; for(i=0; i<10; i=i+1){a=a+2; if(a==6) return a;}'
assert 40 'a=0; b=0; for(i=0; i<10; i=i+1){a=a+1; if(i==9){a=a*2;}} for(j=0; j<10; j=j+1){b=b+1; if(j==9){b=b*2;}} return a+b;'
assert 3 'a=4; if(a==4){a=a+2;if(a==6) {a=a-3; return a;}}'
assert 25 'sum=0; for(i=1;i<=5;i=i+1){for(j=1;j<=5;j=j+1){sum=sum+1;}} return sum;'
assert 5 'a=5; if(a>=0){if(a==5){return a;}}'
assert 10 'a=3; {b=7; if(a==3) return a+b;}'

assert 3 'a=3; if(a==1) return 1; if(a==2) return 2; if(a==3) return 3;'

echo OK

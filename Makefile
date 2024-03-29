# ccに渡すコンパイラオプションを指定
CFLAGS=-std=c11 -g -static
# src/*.cをソースとして指定
SRCS=$(wildcard src/*.c)
# SRCSの末尾を.oに置換した名前のファイルをオブジェクトファイルとして指定
OBJS=$(SRCS:.c=.o)

# 9ccの依存関係を指定し、それに対するルールを設定
9cc: $(OBJS)
				$(CC) -o 9cc $(OBJS) $(LDFLAGS)

# OBJSの依存関係を指定
$(OBJS): src/9cc.h

# testの依存関係を指定し、それに対するルールを設定
test: 9cc
				./test/test_driver.sh

donut: 9cc
				./9cc ./sample/donut.c > tmp.s
				cc -g -O0 -o tmp tmp.s
				./tmp

life_game: 9cc
				./9cc ./sample/life_game.c > tmp.s
				cc -g -O0 -o tmp tmp.s
				./tmp

# cleanに対するルールを設定
clean:
				rm -f 9cc .gdb_history tmp* src/*.o test/*.s
				find ./ -name "peda-session-*.txt" -exec rm {} \;

# testとcleanをダミーターゲット(実際に存在しないファイル)に指定
.PHONY: test clean donut life_game

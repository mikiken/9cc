# ccに渡すコンパイラオプションを指定
CFLAGS=-std=c11 -g -static
# src/*.cをソースとして指定
SRCS=$(wildcard src/*.c)
# SRCSの末尾を.oに置換した名前のファイルをオブジェクトファイルとして指定
OBJS=$(SRCS:.c=.o)
# テストのソースを指定
TEST_SRCS=$(wildcard test/test_src/*.c)

# 9ccの依存関係を指定し、それに対するルールを設定
9cc: $(OBJS)
				$(CC) -o 9cc $(OBJS) $(LDFLAGS)

# OBJSの依存関係を指定
$(OBJS): src/9cc.h

# testの依存関係を指定し、それに対するルールを設定
test: 9cc
				./test/test_driver.sh

# cleanに対するルールを設定
clean:
				rm -f 9cc src/*.o tmp* .gdb_history peda-session-*.txt .vscode/peda-session-*.txt

# testとcleanをダミーターゲット(実際に存在しないファイル)に指定
.PHONY: test clean

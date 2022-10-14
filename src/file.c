#include <errno.h>
#include "9cc.h"

// 指定されたファイルの内容を返す
char *read_file(char *path) {
  // ファイルを開く
  FILE *fp = fopen(path, "r");
  if (!fp)
    error("以下のファイルを開くことができませんでした %s : %s", path, strerror(errno));

  // ファイルの長さを調べる
  if (fseek(fp, 0, SEEK_END) == -1)
    error("%s : fseek: %s", path, strerror(errno));
  size_t size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1)
    error("%s : fseek: %s", path, strerror(errno));

  // ファイルを読み込む
  char *buf = calloc(size + 2, sizeof(char)); // ファイル末尾を'\n\0'終端にするため、size+2の領域を確保しておく
  fread(buf, size, 1, fp);

  // ファイルを必ず"\n\0"終端にする
  if (size == 0 || buf[size - 1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\0';

  fclose(fp);
  return buf;
}
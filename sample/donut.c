// This source code is originally written by @a1k0n,
// and rewritten by @hsjoihs so that it works without floating type.

int putchar();
void *memset();

int m(int a, int b) {
  return (a * b + 5000) / 10000;
}

void a(int *c, int *s, int d, int t) {
  int k;
  int l;
  k = m(*c, d) - m(*s, t);
  l = m(*s, d) + m(*c, t);
  *c = k;
  *s = l;
}

int usleep();
int printf();

int main() {
  int z[1760];
  char b[1760];
  printf("\e[2J");
  int s;
  int q;
  int r;
  int u;
  int v;
  s = 10000;
  q = s;
  r = 0;
  u = s;
  v = 0;
  for (;; a(&q, &r, s - 8, 400), a(&u, &v, s - 2, 200)) {
    memset(b, 32, 1760);
    memset(z, 0, 7040);
    int l;
    int p;
    l = 0;
    p = s;
    int i;
    for (i = 0; i < 88; i++, a(&p, &l, 9974 + i % 2, 714)) {
      int w;
      int e;
      w = 0;
      e = s;
      int j;
      for (j = 0; j < 314; j++, a(&e, &w, s - 2, 200)) {
        int f;
        int g;
        int t;
        int x;
        int y;
        int o;
        int N;
        f = p + 2 * s;
        g = s * s / (m(m(w, f), r) + m(l, q) + 5 * s);
        t = m(m(w, q), f) - m(l, r);
        x = 40 + 30 * m(g, m(m(e, u), f) - m(t, v)) / s;
        y = 12 + 15 * m(g, m(m(e, v), f) + m(t, u)) / s;
        o = x + 80 * y;
        N = 8 * (m(m(l, r) - m(m(w, q), p), u) - m(m(w, r), p) - m(l, q) - m(m(e, v), p)) / s;
        if (y > 0 && g > z[o] && y < 22 && x > 0 && 80 > x) {
          z[o] = g;
          b[o] = ".,-~:;=!*#$@"[N >= 1 ? N : 0] + 0;
        }
      }
    }
    printf("\e[H");
    int k;
    for (k = 0; k < 1761; k++)
      putchar(k % 80 ? b[k] : 10);
    printf("Author: @a1k0n. Rewritten by @hsjoihs so that it works without floating types.\n");
    printf("Note that rounding errors gradually reduce the donut's size.\n");
    usleep(50000);
  }
  return 0;
}
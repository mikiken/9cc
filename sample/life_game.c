// Edited by uint256_t
// https://gist.github.com/maekawatoshiki/8ea2f392e22698593d7ee8522aabbbd0

int printf();
int putchar();
int usleep();

int count_nbr();

int main(void) {
  int neighbour_count[20][20];

  /*
  int grid[20][20] = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  };
  */

  // 上のような初期化式がまだ書けないので、今のところはfor文で代用
  int grid[20][20];
  for (int a = 0; a < 20; a++)
    for (int b = 0; b < 20; b++)
      grid[a][b] = 0;
  for (int c = 5; c < 15; c++)
    grid[7][c] = 1;
  for (int d = 2; d < 6; d++)
    grid[14][d] = 1;

  grid[15][1] = 1;
  grid[15][5] = 1;
  grid[16][5] = 1;
  grid[17][1] = 1;
  grid[17][4] = 1;

  int i;
  int j;
  int steps;

  for (steps = 0; steps < 50; ++steps) {
    printf("\e[0;0H");
    for (i = 0; i < 20; ++i) {
      printf("\n");
      for (j = 0; j < 20; ++j) {
        if (grid[i][j] == 1)
          printf("\e[42m  \e[m");
        else
          printf("\e[47m  \e[m");
        neighbour_count[i][j] = count_nbr(grid, i, j, 20);
      }
    }

    for (i = 0; i < 20; ++i) {
      for (j = 0; j < 20; ++j) {
        if (grid[i][j] >= 1) {
          if (neighbour_count[i][j] <= 1 || neighbour_count[i][j] >= 4)
            grid[i][j] = 0;
        }
        else if (neighbour_count[i][j] == 3)
          grid[i][j] = 1;
      }
    }

    usleep(100000);
  }
  putchar('\n');
  return 0;
}

int count_nbr(int (*grid)[20], int i, int j, int size) {
  int n_count = 0;
  if (i - 1 >= 0 && j - 1 >= 0) {
    if (grid[i - 1][j - 1] >= 1)
      n_count++;
  }

  if (i - 1 >= 0) {
    if (grid[i - 1][j] >= 1)
      n_count++;
  }

  if (i - 1 >= 0 && j + 1 < size) {
    if (grid[i - 1][j + 1] >= 1)
      n_count++;
  }

  if (j - 1 >= 0) {
    if (grid[i][j - 1] >= 1)
      n_count++;
  }

  if (j + 1 < size) {
    if (grid[i][j + 1] >= 1)
      n_count++;
  }

  if (i + 1 < size && j - 1 >= 0) {
    if (grid[i + 1][j - 1] >= 1)
      n_count++;
  }

  if (i + 1 < size) {
    if (grid[i + 1][j] >= 1)
      n_count++;
  }

  if (i + 1 < size && j + 1 < size) {
    if (grid[i + 1][j + 1] >= 1)
      n_count++;
  }

  return n_count;
}
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define A 31
#define B 74
#define M 998244353

#define BUF_SIZE 1024 * 4

int buf[BUF_SIZE];

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "test: wrong format (<size> <x0>)\n");
    exit(0);
  }

  int size = atoi(argv[1]), x0 = atoi(argv[2]);
  if (size <= 0) {
    fprintf(2, "test: <size> must be greater than 0\n");
    exit(0);
  }
  if (size % 4 != 0) {
    fprintf(2, "test: <size> must be divisible by 4\n");
    exit(0);
  }
  if (x0 < 0) {
    fprintf(2, "test: <x0> cannot be less than 0\n");
    exit(0);
  }

  int f = open(argv[1], O_RDWR | O_CREATE), wrote_bytes = 0, x = x0;
  int p = 0;
  for (int i = 0; i < size / 4; ++i) {
    buf[p++] = x;
    x = ((uint64) A * x + B) % M;

    if (p == BUF_SIZE || i == size / 4 - 1) {
      int cur = write(f, buf, p * 4);
      wrote_bytes += cur;
      fprintf(2, "wrote %d\n", wrote_bytes);
      if (cur != p * 4)
        break;
      p = 0;
    }
  }
  close(f);

  printf("total: wrote %d bytes\n", wrote_bytes);

  if (wrote_bytes != size) {
    printf("error: cannot write %d bytes\n", size);
    exit(0);
  }

  f = open(argv[1], O_RDONLY);
  x = x0;
  int read_bytes = 0, corrupted_bytes = 0;
  while (1) {
    int cur = read(f, buf, BUF_SIZE * 4);
    if (cur == 0)
      break;
    read_bytes += cur;
    printf("read %d bytes\n", read_bytes);
    for (int j = 0; j < cur / 4; ++j) {
      if (x != buf[j])
        corrupted_bytes++;
      x = ((uint64) A * x + B) % M;
    }
  }
  close(f);

  printf("total: read %d bytes\n", read_bytes);

  if (read_bytes != wrote_bytes) {
    printf("error: read_bytes != wrote_bytes\n");
    exit(0);
  }

  printf("total: success %d/%d bytes\n", read_bytes - corrupted_bytes, read_bytes);
  if (corrupted_bytes != 0) {
    printf("error: %d corrupted bytes\n", corrupted_bytes);
  }

  exit(0);
}

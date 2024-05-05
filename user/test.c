#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define A 31
#define B 74
#define M 998244353

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
  for (int i = 0; i < size / 4; ++i) {
    int cur = write(f, &x, 4);
    wrote_bytes += cur;
    if (cur != 4)
      break;
    // if ((i + 1) % 10000 == 0)
    //   fprintf(2, "wrote %d\n", wrote_bytes);
    x = ((uint64) A * x + B) % M;
  }
  close(f);

  printf("total: wrote %d bytes\n", wrote_bytes);

  f = open(argv[1], O_RDONLY);
  x = x0;
  int read_bytes = 0, corrupted_bytes = 0;
  for (int i = 0; i < size / 4; ++i) {
    int y, cur = read(f, &y, 4);
    read_bytes += cur;
    if (cur != 4)
      break;
    if (x != y)
      corrupted_bytes++;
    // if ((i + 1) % 10000 == 0)
    //  fprintf(2, "read %d\n", read_bytes);
    x = ((uint64) A * x + B) % M;
  }
  close(f);

  printf("total: read %d bytes\n", read_bytes);
  printf("total: success %d/%d bytes\n", read_bytes - corrupted_bytes, read_bytes);

  exit(0);
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_ITER (1 << 20)

int
main(int argc, char *argv[])
{
  int fds[2], response = pipe(fds);
  if (response < 0) {
    printf("pipe failed (response = %d)\n", response);
    exit(1);
  }

  int cid = fork();
  if (cid < 0) {
    printf("fork failed (response = %d)\n", cid);

    response = close(fds[0]);
    if (response < 0)
      printf("close failed (response = %d)\n", response);

    response = close(fds[1]);
    if (response < 0)
      printf("close failed (response = %d)\n", response);

    exit(1);
  }

  if (cid != 0) {
    response = close(fds[0]);
    if (response < 0)
      printf("close failed (response = %d)\n", response);

    int already_wrote = 0, iter = 0;
    for (int i = 1; i < argc; ++i) {
      int len = strlen(argv[i]) - already_wrote;

      response = write(fds[1], argv[i] + already_wrote, len);
      if (response < 0) {
        printf("write failed (response = %d)\n", response);

        response = close(fds[1]);
        if (response < 0)
          printf("close failed (response = %d)\n", response);

        exit(1);
      }

      if (response != len) {
        if (response == 0)
          iter++;
        if (iter == MAX_ITER) {
          printf("write failed (unknown error)\n");

          response = close(fds[1]);
          if (response < 0)
            printf("close failed (response = %d)\n", response);

          exit(1);
        }

        already_wrote += response;
        --i;
        continue;
      }

      iter = 0;

      if (i + 1 < argc) {
        do {
          ++iter;
          response = write(fds[1], " ", 1);
          if (response < 0) {
            printf("write failed (response = %d)\n", response);

            response = close(fds[1]);
            if (response < 0)
              printf("close failed (response = %d)\n", response);

            exit(1);
          }
        } while (response != 1 && iter < MAX_ITER);

        if (response != 1) {
          printf("write failed (unknown error)\n");

          response = close(fds[1]);
          if (response < 0)
            printf("close failed (response = %d)\n", response);

          exit(1);
        }
      }
    }

    iter = 0;
    do {
      ++iter;
      response = write(fds[1], "\n", 1);
      if (response < 0) {
        printf("write failed (response = %d)\n", response);

        response = close(fds[1]);
        if (response < 0)
          printf("close failed (response = %d)\n", response);

        exit(1);
      }
    } while (response != 1 && iter < MAX_ITER);

    if (response != 1) {
      printf("write failed (unknown error)\n");

      response = close(fds[1]);
      if (response < 0)
        printf("close failed (response = %d)\n", response);

      exit(1);
    }

    response = close(fds[1]);
    if (response < 0)
      printf("close failed (response = %d)\n", response);

    response = wait((int *) 0);
    if (response < 0) {
      printf("wait failed (response = %d)\n", response);
      exit(1);
    }
  } else {
    response = close(fds[1]);
    if (response < 0)
      printf("close failed (response = %d)\n", response);

    char c;
    int iter = 0;
    do {
      response = read(fds[0], &c, 1);
      if (response < 0) {
        printf("read failed (response = %d)\n", response);

        response = close(fds[0]);
        if (response < 0)
          printf("close failed (response = %d)\n", response);

        exit(1);
      }

      if (response == 0) {
        ++iter;
        continue;
      }

      iter = 0;
      printf("%c", c);
    } while (c != '\n' && iter < MAX_ITER);

    response = close(fds[0]);
    if (response < 0)
      printf("close failed (response = %d)\n", response);

    if (iter == MAX_ITER) {
      printf("read failed (unknown error)\n");
      exit(1);
    }
  }

  exit(0);
}

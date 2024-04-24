#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void exit_error(char *message) {
  printf("error: %s\n", message);
  exit(1);
}

int
main(int argc, char *argv[]) {
  if (argc <= 1)
    exit_error("not enough args");

  int fd1[2];
  if (pipe(fd1) < 0)
    exit_error("failed to pipe");

  int fd2[2];
  if (pipe(fd2) < 0) {
    close(fd1[0]);
    close(fd2[0]);
    exit_error("failed to pipe");
  }

  int cid = fork();
  if (cid < 0) {
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    exit_error("failed to fork");
  }

  int pid = getpid();

  if (cid > 0) {
    if (close(fd1[0]) < 0) {
      close(fd1[1]);
      close(fd2[0]);
      close(fd2[1]);
      exit_error("failed to close fd1[0] (parent)");
    }
    if (close(fd2[1]) < 0) {
      close(fd1[1]);
      close(fd2[0]);
      exit_error("failed to close fd2[1] (parent)");
    }

    if (write(fd1[1], argv[1], strlen(argv[1])) != strlen(argv[1])) {
      close(fd1[1]);
      close(fd2[0]);
      exit_error("failed to write (parent)");
    }
    if (close(fd1[1]) < 0) {
      close(fd2[0]);
      exit_error("failed to close fd1[1] (parent)");
    }

    char c;
    while (1) {
      int k = read(fd2[0], &c, 1);
      if (k < 0) {
        close(fd2[0]);
        exit_error("failed to read (parent)");
      }

      if (k == 0)
        break;

      printf("%d: received '%c'\n", pid, c);
    }

    if (close(fd2[0]) < 0)
      exit_error("failed to close fd2[0] (parent)");

    exit(0);
  } else {
    if (close(fd1[1]) < 0) {
      close(fd1[0]);
      close(fd2[0]);
      close(fd2[1]);
      exit_error("failed to close fd1[1] (child)");
    }
    if (close(fd2[0]) < 0) {
      close(fd1[0]);
      close(fd2[1]);
      exit_error("failed to close fd2[0] (child)");
    }

    char c;
    while (1) {
      int k = read(fd1[0], &c, 1);
      if (k < 0) {
        close(fd1[0]);
        close(fd2[1]);
        exit_error("failed to read (child)");
      }

      if (k == 0)
        break;

      printf("%d: received '%c'\n", pid, c);

      if (write(fd2[1], &c, 1) != 1) {
        close(fd1[0]);
        close(fd2[1]);
        exit_error("failed to write (child)");
      }
    }

    if (close(fd1[0]) < 0) {
      close(fd2[1]);
      exit_error("failed to close fd1[0] (child)");
    }
    if (close(fd2[1]) < 0)
      exit_error("failed to close fd2[1] (child)");

    exit(0);
  }
}


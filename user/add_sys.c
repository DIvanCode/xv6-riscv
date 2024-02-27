#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
  const int BUF_SIZE = 512;

  char buf[BUF_SIZE];
  int ptr = 0;
  gets(buf, BUF_SIZE);

  const char *a = (char*) (buf + ptr);
  if (ptr < BUF_SIZE && (buf[ptr] == '-' || buf[ptr] == '+')) {
    ++ptr;
  }
  while (ptr < BUF_SIZE && '0' <= buf[ptr] && buf[ptr] <= '9') {
    if (ptr > 11) {
      fprintf(2, "add: first number is too large\n");
      exit(1);
    }
    ++ptr;
  }

  if (buf[ptr] != ' ') {
    fprintf(2, "add: input string must contain two numbers and a space between them, (found after first: %d)\n", (int) buf[ptr]);
    exit(1);
  }
  buf[ptr++] = '\0';

  const char *b = (char*) (buf + ptr);
  if (ptr < BUF_SIZE && (buf[ptr] == '-' || buf[ptr] == '+')) {
    ++ptr;
  }
  while (ptr < BUF_SIZE && '0' <= buf[ptr] && buf[ptr] <= '9') {
    if (ptr > 11) {
      fprintf(2, "add: second number is too large\n");
      exit(1);
    }
    ++ptr;
  }

  // 0..31 and 127 - control characters
  if (31 < buf[ptr] && buf[ptr] < 127) {
    fprintf(2, "add: input string must contain two numbers and a space between them, (found after second: %d)\n", (int) buf[ptr]);
    exit(1);
  }
  buf[ptr] = '\0';

  printf("%d\n", sys_add(atoi(a), atoi(b)));

  exit(0);
}

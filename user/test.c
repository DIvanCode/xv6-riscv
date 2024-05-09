#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

void mkfile(char *filename, char *data) {
  int f = open(filename, O_RDWR | O_CREATE);
  write(f, data, strlen(data));
  close(f);
}

int errors = 0;

void error(int test, int test_case) {
  errors++;
  fprintf(2, "test %d: error #%d\n", test, test_case);
}

void ok(int test) {
  if (errors == 0)
    fprintf(2, "test %d: ok\n", test);
  errors = 0;
}

void test1() {
  int test = 1;

  // Arrange
  mkdir("/test1");
  mkfile("/test1/a", "data");

  // Act
  symlink("/test1/a", "/test1/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test1/b", b_data);
  if (strcmp(b_data, "/test1/a") != 0)
    error(test, 1);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fb = open("/test1/b", O_RDONLY);
  read(fb, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 2);
  close(fb);

  ok(test);
}

void test2() {
  int test = 2;

  // Arrange
  mkdir("/test2");
  mkfile("/test2/a", "data");

  // Act
  symlink("a", "/test2/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test2/b", b_data);
  if (strcmp(b_data, "a") != 0)
    error(test, 1);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fb = open("/test2/b", O_RDONLY);
  read(fb, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 2);
  close(fb);

  ok(test);
}

void test3() {
  int test = 3;

  // Arrange
  mkdir("/test3");
  mkfile("/test3/a", "data");
  mkdir("/test3/d1");
  mkdir("/test3/d1/d2");
  mkdir("/test3/d1/d2/d3");

  // Act
  symlink("../../../a", "/test3/d1/d2/d3/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test3/d1/d2/d3/b", b_data);
  if (strcmp(b_data, "../../../a") != 0)
    error(test, 1);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fb = open("/test3/d1/d2/d3/b", O_RDONLY);
  read(fb, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 2);
  close(fb);

  ok(test);
}

void test4() {
  int test = 4;

  // Arrange
  mkdir("/test4");
  mkdir("/test4/d1");
  mkdir("/test4/d1/d2");
  mkdir("/test4/d1/d2/d3");
  mkfile("/test4/d1/d2/d3/a", "data");

  // Act
  symlink("d1/d2/d3/a", "/test4/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test4/b", b_data);
  if (strcmp(b_data, "d1/d2/d3/a") != 0)
    error(test, 1);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fb = open("/test4/b", O_RDONLY);
  read(fb, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 2);
  close(fb);

  ok(test);
}

void test5() {
  int test = 5;

  // Arrange
  mkdir("/test5");
  mkfile("/test5/a", "data");

  // Act
  symlink("/test5/a", "/test5/b");
  symlink("/test5/b", "/test5/c");
  symlink("/test5/c", "/test5/d");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test5/b", b_data);
  if (strcmp(b_data, "/test5/a") != 0)
    error(test, 1);

  char c_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    c_data[i] = 0;
  readlink("/test5/c", c_data);
  if (strcmp(c_data, "/test5/b") != 0)
    error(test, 2);

  char d_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    d_data[i] = 0;
  readlink("/test5/d", d_data);
  if (strcmp(d_data, "/test5/c") != 0)
    error(test, 3);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fd = open("/test5/d", O_RDONLY);
  read(fd, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 4);
  close(fd);

  ok(test);
}

void test6() {
  int test = 6;

  // Arrange
  mkdir("/test6");
  mkfile("/test6/a", "data");

  // Act
  symlink("/test6/a", "/test6/b");
  symlink("b", "/test6/c");
  symlink("/test6/c", "/test6/d");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test6/b", b_data);
  if (strcmp(b_data, "/test6/a") != 0)
    error(test, 1);

  char c_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    c_data[i] = 0;
  readlink("/test6/c", c_data);
  if (strcmp(c_data, "b") != 0)
    error(test, 2);

  char d_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    d_data[i] = 0;
  readlink("/test6/d", d_data);
  if (strcmp(d_data, "/test6/c") != 0)
    error(test, 3);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fd = open("/test6/d", O_RDONLY);
  read(fd, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 4);
  close(fd);

  ok(test);
}

void test7() {
  int test = 7;

  // Arrange
  mkdir("/test7");
  mkfile("/test7/a", "data");

  // Act
  symlink("a", "/test7/b");
  symlink("b", "/test7/c");
  symlink("c", "/test7/d");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test7/b", b_data);
  if (strcmp(b_data, "a") != 0)
    error(test, 1);

  char c_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    c_data[i] = 0;
  readlink("/test7/c", c_data);
  if (strcmp(c_data, "b") != 0)
    error(test, 2);

  char d_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    d_data[i] = 0;
  readlink("/test7/d", d_data);
  if (strcmp(d_data, "c") != 0)
    error(test, 3);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fd = open("/test7/d", O_RDONLY);
  read(fd, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 4);
  close(fd);

  ok(test);
}

void test8() {
  int test = 8;

  // Arrange
  mkdir("/test8");

  // Act
  symlink("b", "/test8/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test8/b", b_data);
  if (strcmp(b_data, "b") != 0)
    error(test, 1);

  int fb = open("/test8/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    close(fb);
  }

  ok(test);
}

void test9() {
  int test = 9;

  // Arrange
  mkdir("/test9");
  mkfile("/test9/a", "data");

  // Act
  symlink("a", "/test9/b");
  symlink("b", "/test9/c");
  symlink("c", "/test9/d");
  symlink("d", "/test9/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test9/b", b_data);
  if (strcmp(b_data, "d") != 0)
    error(test, 1);

  char c_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    c_data[i] = 0;
  readlink("/test9/c", c_data);
  if (strcmp(c_data, "b") != 0)
    error(test, 2);

  char d_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    d_data[i] = 0;
  readlink("/test9/d", d_data);
  if (strcmp(d_data, "c") != 0)
    error(test, 3);

  int fb = open("/test9/b", O_RDONLY);
  if (fb != -1) {
    error(test, 4);
    close(fb);
  }

  ok(test);
}

void test10() {
  int test = 10;

  // Arrange
  mkdir("/test10");

  // Act
  symlink("/test10/a", "/test10/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test10/b", b_data);
  if (strcmp(b_data, "/test10/a") != 0)
    error(test, 1);

  int fb = open("/test10/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    close(fb);
  }

  ok(test);
}

void test11() {
  int test = 11;

  // Arrange
  mkdir("/test11");
  mkdir("/test11/d1");
  mkdir("/test11/d1/d2");
  mkdir("/test11/d1/d2/d3");
  mkfile("/test11/d1/d2/d3/a", "data");

  // Act
  symlink("a", "/test11/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test11/b", b_data);
  if (strcmp(b_data, "a") != 0)
    error(test, 1);

  int fb = open("/test11/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    close(fb);
  }

  ok(test);
}

void test12() {
  int test = 12;

  // Arrange
  mkdir("/test12");
  mkdir("/test12/d1");
  mkdir("/test12/d1/d2");
  mkdir("/test12/d1/d2/d3");
  mkfile("/test12/d1/d2/d3/a", "data");

  // Act
  symlink("../../../a", "/test12/d1/d2/d3/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test12/d1/d2/d3/b", b_data);
  if (strcmp(b_data, "../../../a") != 0)
    error(test, 1);

  int fb = open("/test12/d1/d2/d3/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    close(fb);
  }

  ok(test);
}

void test13() {
  int test = 13;

  // Arrange
  mkdir("/test13");
  mkfile("/test13/a", "data");
  mkdir("/test13/d1");
  mkdir("/test13/d1/d2");
  mkdir("/test13/d1/d2/d3");

  // Act
  symlink("d1/d2/d3/a", "/test13/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  readlink("/test13/b", b_data);
  if (strcmp(b_data, "d1/d2/d3/a") != 0)
    error(test, 1);

  int fb = open("/test13/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    close(fb);
  }

  ok(test);
}

static void (*tests[])(void) = {
  test1,
  test2,
  test3,
  test4,
  test5,
  test6,
  test7,
  test8,
  test9,
  test10,
  test11,
  test12,
  test13
};

int main(int argc, char *argv[]) {
  if (argc == 1) {
    for (int i = 1; i <= 13; ++i)
      tests[i - 1]();
  } else if (argc == 2) {
    int test = atoi(argv[1]);
    if (test < 1 || test > 13) {
      fprintf(2, "error: test must be between 1 and 13\n");
      exit(0);
    }
    tests[test - 1]();
  }

  exit(0);
}


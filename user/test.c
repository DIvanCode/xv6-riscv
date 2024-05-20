#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/fs.h"
#include "user/user.h"

void view_tree(char *, int);

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

int safe_open(char *filename, int flags) {
  int f = open(filename, flags);
  if (f < 0) {
    fprintf(2, "error: open\n");
    exit(1);
  }
  return f;
}

void safe_read(int f, char *data, int size) {
  if (read(f, data, size) < 0) {
    fprintf(2, "error: read\n");
    exit(1);
  }
}

void safe_write(int f, char *data, int size) {
  if (write(f, data, size) != size) {
    fprintf(2, "error: write\n");
    exit(1);
  }
}

void safe_close(int f) {
  if (close(f) < 0) {
    fprintf(2, "error: close\n");
    exit(1);
  }
}

void safe_mkfile(char *filename, char *data) {
  int f = safe_open(filename, O_RDWR | O_CREATE);
  safe_write(f, data, strlen(data));
  safe_close(f);
}

void safe_mkdir(char *name) {
  if (mkdir(name) < 0) {
    fprintf(2, "error: mkdir\n");
    exit(1);
  }
}

void safe_symlink(char *target, char *filename) {
  if (symlink(target, filename) < 0) {
    fprintf(2, "error: symlink\n");
    exit(1);
  }
}

void safe_readlink(char *filename, char *data) {
  if (readlink(filename, data) < 0) {
    fprintf(2, "error: readlink\n");
    exit(1);
  }
}

void test1() {
  int test = 1;

  // Arrange
  safe_mkdir("/test1");
  safe_mkfile("/test1/a", "data");

  // Act
  safe_symlink("/test1/a", "/test1/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test1/b", b_data);
  if (strcmp(b_data, "/test1/a") != 0)
    error(test, 1);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fb = safe_open("/test1/b", O_RDONLY);
  safe_read(fb, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 2);
  safe_close(fb);

  ok(test);
}

void test2() {
  int test = 2;

  // Arrange
  safe_mkdir("/test2");
  safe_mkfile("/test2/a", "data");

  // Act
  safe_symlink("a", "/test2/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test2/b", b_data);
  if (strcmp(b_data, "a") != 0)
    error(test, 1);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fb = safe_open("/test2/b", O_RDONLY);
  safe_read(fb, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 2);
  safe_close(fb);

  ok(test);
}

void test3() {
  int test = 3;

  // Arrange
  safe_mkdir("/test3");
  safe_mkfile("/test3/a", "data");
  safe_mkdir("/test3/d1");
  safe_mkdir("/test3/d1/d2");
  safe_mkdir("/test3/d1/d2/d3");

  // Act
  safe_symlink("../../../a", "/test3/d1/d2/d3/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test3/d1/d2/d3/b", b_data);
  if (strcmp(b_data, "../../../a") != 0)
    error(test, 1);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fb = safe_open("/test3/d1/d2/d3/b", O_RDONLY);
  safe_read(fb, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 2);
  safe_close(fb);

  ok(test);
}

void test4() {
  int test = 4;

  // Arrange
  safe_mkdir("/test4");
  safe_mkdir("/test4/d1");
  safe_mkdir("/test4/d1/d2");
  safe_mkdir("/test4/d1/d2/d3");
  safe_mkfile("/test4/d1/d2/d3/a", "data");

  // Act
  safe_symlink("d1/d2/d3/a", "/test4/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test4/b", b_data);
  if (strcmp(b_data, "d1/d2/d3/a") != 0)
    error(test, 1);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fb = safe_open("/test4/b", O_RDONLY);
  safe_read(fb, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 2);
  safe_close(fb);

  ok(test);
}

void test5() {
  int test = 5;

  // Arrange
  safe_mkdir("/test5");
  safe_mkfile("/test5/a", "data");

  // Act
  safe_symlink("/test5/a", "/test5/b");
  safe_symlink("/test5/b", "/test5/c");
  safe_symlink("/test5/c", "/test5/d");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test5/b", b_data);
  if (strcmp(b_data, "/test5/a") != 0)
    error(test, 1);

  char c_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    c_data[i] = 0;
  safe_readlink("/test5/c", c_data);
  if (strcmp(c_data, "/test5/b") != 0)
    error(test, 2);

  char d_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    d_data[i] = 0;
  safe_readlink("/test5/d", d_data);
  if (strcmp(d_data, "/test5/c") != 0)
    error(test, 3);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fd = safe_open("/test5/d", O_RDONLY);
  safe_read(fd, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 4);
  safe_close(fd);

  ok(test);
}

void test6() {
  int test = 6;

  // Arrange
  safe_mkdir("/test6");
  safe_mkfile("/test6/a", "data");

  // Act
  safe_symlink("/test6/a", "/test6/b");
  safe_symlink("b", "/test6/c");
  safe_symlink("/test6/c", "/test6/d");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test6/b", b_data);
  if (strcmp(b_data, "/test6/a") != 0)
    error(test, 1);

  char c_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    c_data[i] = 0;
  safe_readlink("/test6/c", c_data);
  if (strcmp(c_data, "b") != 0)
    error(test, 2);

  char d_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    d_data[i] = 0;
  safe_readlink("/test6/d", d_data);
  if (strcmp(d_data, "/test6/c") != 0)
    error(test, 3);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fd = safe_open("/test6/d", O_RDONLY);
  safe_read(fd, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 4);
  safe_close(fd);

  ok(test);
}

void test7() {
  int test = 7;

  // Arrange
  safe_mkdir("/test7");
  safe_mkfile("/test7/a", "data");

  // Act
  safe_symlink("a", "/test7/b");
  safe_symlink("b", "/test7/c");
  safe_symlink("c", "/test7/d");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test7/b", b_data);
  if (strcmp(b_data, "a") != 0)
    error(test, 1);

  char c_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    c_data[i] = 0;
  safe_readlink("/test7/c", c_data);
  if (strcmp(c_data, "b") != 0)
    error(test, 2);

  char d_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    d_data[i] = 0;
  safe_readlink("/test7/d", d_data);
  if (strcmp(d_data, "c") != 0)
    error(test, 3);

  char data[16];
  for (int i = 0; i < 16; ++i)
    data[i] = 0;
  int fd = safe_open("/test7/d", O_RDONLY);
  safe_read(fd, data, 16);
  if (strcmp(data, "data") != 0)
    error(test, 4);
  safe_close(fd);

  ok(test);
}

void test8() {
  int test = 8;

  // Arrange
  safe_mkdir("/test8");

  // Act
  safe_symlink("b", "/test8/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test8/b", b_data);
  if (strcmp(b_data, "b") != 0)
    error(test, 1);

  int fb = open("/test8/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    safe_close(fb);
  }

  ok(test);
}

void test9() {
  int test = 9;

  // Arrange
  safe_mkdir("/test9");
  safe_mkfile("/test9/a", "data");

  // Act
  safe_symlink("a", "/test9/b");
  safe_symlink("b", "/test9/c");
  safe_symlink("c", "/test9/d");
  safe_symlink("d", "/test9/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test9/b", b_data);
  if (strcmp(b_data, "d") != 0)
    error(test, 1);

  char c_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    c_data[i] = 0;
  safe_readlink("/test9/c", c_data);
  if (strcmp(c_data, "b") != 0)
    error(test, 2);

  char d_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    d_data[i] = 0;
  safe_readlink("/test9/d", d_data);
  if (strcmp(d_data, "c") != 0)
    error(test, 3);

  int fb = open("/test9/b", O_RDONLY);
  if (fb != -1) {
    error(test, 4);
    safe_close(fb);
  }

  ok(test);
}

void test10() {
  int test = 10;

  // Arrange
  safe_mkdir("/test10");

  // Act
  safe_symlink("/test10/a", "/test10/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test10/b", b_data);
  if (strcmp(b_data, "/test10/a") != 0)
    error(test, 1);

  int fb = open("/test10/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    safe_close(fb);
  }

  ok(test);
}

void test11() {
  int test = 11;

  // Arrange
  safe_mkdir("/test11");
  safe_mkdir("/test11/d1");
  safe_mkdir("/test11/d1/d2");
  safe_mkdir("/test11/d1/d2/d3");
  safe_mkfile("/test11/d1/d2/d3/a", "data");

  // Act
  safe_symlink("a", "/test11/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test11/b", b_data);
  if (strcmp(b_data, "a") != 0)
    error(test, 1);

  int fb = open("/test11/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    safe_close(fb);
  }

  ok(test);
}

void test12() {
  int test = 12;

  // Arrange
  safe_mkdir("/test12");
  safe_mkdir("/test12/d1");
  safe_mkdir("/test12/d1/d2");
  safe_mkdir("/test12/d1/d2/d3");
  safe_mkfile("/test12/d1/d2/d3/a", "data");

  // Act
  safe_symlink("../../../a", "/test12/d1/d2/d3/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test12/d1/d2/d3/b", b_data);
  if (strcmp(b_data, "../../../a") != 0)
    error(test, 1);

  int fb = open("/test12/d1/d2/d3/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    safe_close(fb);
  }

  ok(test);
}

void test13() {
  int test = 13;

  // Arrange
  safe_mkdir("/test13");
  safe_mkfile("/test13/a", "data");
  safe_mkdir("/test13/d1");
  safe_mkdir("/test13/d1/d2");
  safe_mkdir("/test13/d1/d2/d3");

  // Act
  safe_symlink("d1/d2/d3/a", "/test13/b");

  // Assert
  char b_data[MAXPATH];
  for (int i = 0; i < MAXPATH; ++i)
    b_data[i] = 0;
  safe_readlink("/test13/b", b_data);
  if (strcmp(b_data, "d1/d2/d3/a") != 0)
    error(test, 1);

  int fb = open("/test13/b", O_RDONLY);
  if (fb != -1) {
    error(test, 2);
    safe_close(fb);
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

  view_tree(".", 0);

  exit(0);
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
view_tree(char *path, int level)
{
  if (level == 8)
    return;
  char mar[8];
  for (int i = 0; i < level; ++i)
    mar[i] = ' ';
  mar[level] = 0;
  char buf[64];
  char *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_NOFOLLOW)) < 0){
    fprintf(2, "%sview_tree: cannot open %s\n", mar, path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "%sview_tree: cannot stat %s\n", mar, path);
    safe_close(fd);
    return;
  }

  char link_buf[MAXPATH];
  int r;

  switch(st.type){
    case T_DEVICE:
    case T_SYMLINK:
      r = readlink(path, link_buf);
      if (r < 0)
        printf("%s%s %d %d %l ->?\n", mar, fmtname(path), st.type, st.ino, st.size);
      else {
        if (r < MAXPATH)
          link_buf[r] = 0;
        printf("%s%s %d %d %l ->%s\n", mar, fmtname(path), st.type, st.ino, st.size, link_buf);
      }
      break;

    case T_FILE:
      printf("%s%s %d %d %l\n", mar, fmtname(path), st.type, st.ino, st.size);
      break;

    case T_DIR:
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("%sview_tree: path too long\n", mar);
        break;
      }
      strcpy(buf, path);
      p = buf+strlen(buf);
      *p++ = '/';
      while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
          continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(lstat(buf, &st) < 0){
          printf("%sview_tree: cannot stat %s\n", mar, buf);
          continue;
        }
        if (st.type == T_SYMLINK) {
          r = readlink(buf, link_buf);
          if (r < 0)
            printf("%s%s %d %d %l ->?\n", mar, fmtname(buf), st.type, st.ino, st.size);
          else {
            if (r < MAXPATH)
              link_buf[r] = 0;
            printf("%s%s %d %d %l ->%s\n", mar, fmtname(buf), st.type, st.ino, st.size, link_buf);
          }
        } else if (level == 0 || buf[strlen(buf) - 1] != '.')
          printf("%s%s %d %d %d\n", mar, fmtname(buf), st.type, st.ino, st.size);
        if (st.type == T_DIR && buf[strlen(buf) - 1] != '.')
          view_tree(buf, level+1);
      }
      break;
  }
  safe_close(fd);
}


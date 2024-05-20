#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define TYPE int
#define PAGES 3
#define PERPAGE PGSIZE / sizeof(TYPE)
#define SIZE PAGES * PERPAGE

TYPE a[SIZE];
int failed;

#define assert(exp) if (!(exp)) { fprintf(2, "(Line %d) assertion failed: %s\n", __LINE__, #exp); failed++; }

void TEST_1_array_status_0_before_change() {
  // Arrange
  pgaccess(a + PERPAGE, PERPAGE * sizeof(TYPE), 0);
  int status = 0;

  // Act
  pgaccess(a + PERPAGE, PERPAGE * sizeof(TYPE), &status);

  // Assert
  assert(status == 0);
}

void TEST_2_array_status_1_after_change() {
  // Arrange
  pgaccess(a + PERPAGE, PERPAGE * sizeof(TYPE), 0);
  int status = 0;

  // Act
  a[PERPAGE] = 0;
  pgaccess(a + PERPAGE, PERPAGE * sizeof(TYPE), &status);

  // Assert
  assert(status == 1);
}

void TEST_3_array_status_0_after_change_and_pgaccess() {
  // Arrange
  pgaccess(a + PERPAGE, PERPAGE * sizeof(TYPE), 0);
  int status = 0;

  // Act
  a[PERPAGE] = 0;
  pgaccess(a + PERPAGE, PERPAGE * sizeof(TYPE), 0);
  pgaccess(a + PERPAGE, PERPAGE * sizeof(TYPE), &status);

  // Assert
  assert(status == 0);
}

void TEST_4_int_status_0_before_change() {
  // Arrange
  int x = 0, status = 0;
  pgaccess(&x, sizeof(int), 0);

  // Act
  pgaccess(&x, sizeof(int), &status);

  // Assert
  assert(status == 0);
}

void TEST_5_int_status_1_after_change() {
  // Arrange
  int x = 0, status = 0;
  pgaccess(&x, sizeof(int), 0);

  // Act
  x = 0;
  pgaccess(&x, sizeof(int), &status);

  // Assert
  assert(status == 1);
}

void TEST_6_int_status_0_after_change_and_pgaccess() {
  // Arrange
  int x = 0, status = 0;
  pgaccess(&x, sizeof(int), 0);

  // Act
  x = 0;
  pgaccess(&x, sizeof(int), &status);
  pgaccess(&x, sizeof(int), &status);

  // Assert
  assert(status == 0);
}

void TEST_7_array_next_page_status_1_after_change() {
  // Arrange
  pgaccess(a + PERPAGE, 2 * PERPAGE * sizeof(TYPE), 0);
  int status = 0;

  // Act
  a[2 * PERPAGE] = 0;
  pgaccess(a + PERPAGE, 2 * PERPAGE * sizeof(TYPE), &status);

  // Assert
  assert(status == 1);
}

void TEST_8_array_next_page_status_0_after_change_and_pgaccess() {
  // Arrange
  pgaccess(a + PERPAGE, 2 * PERPAGE * sizeof(TYPE), 0);
  int status = 0;

  // Act
  a[2 * PERPAGE] = 0;
  pgaccess(a + PERPAGE, 2 * PERPAGE * sizeof(TYPE), &status);
  pgaccess(a + PERPAGE, 2 * PERPAGE * sizeof(TYPE), &status);

  // Assert
  assert(status == 0);
}

#define TESTS 8

static void (*tests[])(void) = {
  TEST_1_array_status_0_before_change,
  TEST_2_array_status_1_after_change,
  TEST_3_array_status_0_after_change_and_pgaccess,
  TEST_4_int_status_0_before_change,
  TEST_5_int_status_1_after_change,
  TEST_6_int_status_0_after_change_and_pgaccess,
  TEST_7_array_next_page_status_1_after_change,
  TEST_8_array_next_page_status_0_after_change_and_pgaccess
};

void run_test(int t) {
  fprintf(2, "Test #%d...\n", t);
  int failed_before = failed;
  tests[t - 1]();
  if (failed_before == failed)
    fprintf(2, "OK\n");
  else
    fprintf(2, "Failed\n");
  fprintf(2, "\n");
}

void test() {
  for (int t = 1; t <= TESTS; ++t)
    run_test(t);
  if (!failed)
    fprintf(2, "Passed %d tests\n", TESTS);
  else
    fprintf(2, "Failed: %d errors occurred\n", failed);
}

int
main(int argc, char *argv[]) {
  failed = 0;
  if (argc == 1)
    test();
  else if (argc == 2) {
    int t = atoi(argv[1]);
    if (t <= 0 || t > TESTS) {
      fprintf(2, "%d is not correct test\n", t);
      exit(1);
    }
    run_test(t);
  } else {
    fprintf(2, "incorrect format\n");
    exit(1);
  }
  exit(0);
}

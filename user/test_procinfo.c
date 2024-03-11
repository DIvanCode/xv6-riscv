#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "kernel/param.h"
#include "user/user.h"

int test_ok() {
  struct procinfo pinfo[NPROC];
  int cnt_proc = ps_listinfo(pinfo, NPROC);
  return cnt_proc > 0;
}

int test_bad_address() {
  int cnt_proc = ps_listinfo(0, NPROC);
  return cnt_proc > 0;
}

int test_not_enough_space() {
  struct procinfo pinfo[1];
  int cnt_proc = ps_listinfo(pinfo, 1);
  return cnt_proc == -1;
}

int
main(void) {
  int failed = 0;
  if (!test_ok()) {
    ++failed;
    printf("test_ok failed\n");
  }
  if (!test_bad_address()) {
    ++failed;
    printf("test_bad_address failed\n");
  }
  if (!test_not_enough_space()) {
    ++failed;
    printf("test_not_enough_space failed\n");
  }
  if (!failed) {
    printf("tests passed\n");
  }
  exit(0);
}

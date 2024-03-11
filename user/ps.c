#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(void) {
  struct procinfo pinfo[NPROC];
  int cnt_proc = ps_listinfo(pinfo, NPROC);
  static char *states[] = {
    [STATE_UNUSED]    "UNUSED  ",
    [STATE_USED]      "USED    ",
    [STATE_SLEEPING]  "SLEEPING",
    [STATE_RUNNABLE]  "RUNNABLE",
    [STATE_RUNNING]   "RUNNING ",
    [STATE_ZOMBIE]    "ZOMBIE  "
  };
  for (int i = 0; i < cnt_proc; ++i) {
    printf("%s %d %s\n", states[pinfo[i].state], pinfo[i].parent_pid, pinfo[i].name);
  }
  exit(0);
}

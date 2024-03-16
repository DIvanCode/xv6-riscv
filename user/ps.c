#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(void) {
  int n_proc = NPROC;

  while (1) {
    struct procinfo pinfo[n_proc];
    int cnt_proc = ps_listinfo(pinfo, n_proc);

    if (cnt_proc > 0) {
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

      break;
    } else if (cnt_proc == -1) {
      n_proc *= 2;
    } else {
      printf("ps: error\n");
      exit(1);
    }
  }

  exit(0);
}

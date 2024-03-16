#pragma once

enum procinfostate {
    STATE_UNUSED,
    STATE_USED,
    STATE_SLEEPING,
    STATE_RUNNABLE,
    STATE_RUNNING,
    STATE_ZOMBIE
};

struct procinfo {
    char name[16];
    enum procinfostate state;
    int parent_pid;
};

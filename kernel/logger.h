struct logger {
    struct spinlock lock;

    // -1  enabled endlessly
    // 0   disabled
    // x   last tick is x
    int end_ticks[N_TLOGS];
};
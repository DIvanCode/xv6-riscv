struct logger {
    struct spinlock lock;

    // -1  enabled endlessly
    // 0   disabled
    // x   last tick is x
    int end_ticks;
};

#define log_info(fmt, ...) do { if (logger_works()) pr_msg(fmt, ##__VA_ARGS__); } while(0)
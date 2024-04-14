struct queue {
    struct spinlock lock;
    char data[PGSIZE * QNPAGES];
    int over, tail;
};

struct mutex {
  struct sleeplock *lock;
  int ref;
};

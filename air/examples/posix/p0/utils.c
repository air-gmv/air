#include "utils.h"

void add_timespec (struct timespec *s,
                   const struct timespec *t1,
                   const struct timespec *t2)
{
  s->tv_sec  = t1->tv_sec  + t2->tv_sec;
  s->tv_nsec = t1->tv_nsec + t2->tv_nsec;
  s->tv_sec += s->tv_nsec/1000000000;
  s->tv_nsec %= 1000000000;
}

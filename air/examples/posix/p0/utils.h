#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <time.h>

/*****************************************************************************/
/* Helper macros */

#define CHECK_NZ(X) if ((X) != 0) { printf ("%s %d:\n", __FILE__, __LINE__); perror ("[ERROR]"); }
/* Return an error if X is not null, e.g. a return from a syscall */

void add_timespec (struct timespec *s,
                   const struct timespec *t1,
                   const struct timespec *t2);
/* Add two timespec constants, and store the result in s */

#endif /* __UTILS_H__ */

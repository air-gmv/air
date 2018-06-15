/**
 * minteger.h: defines a mutex-protected integer variable
 */

#ifndef _MINTEGER_H_
#define _MINTEGER_H_

#include <pthread.h>

typedef struct s_m_integer {
  int value;
  pthread_mutex_t mutex;
} *m_integer;

m_integer MI_init(int priority);
/* Initialize a m_integer entity, priority is used to configure the mutex */

void MI_write(m_integer m, int v);
int MI_read(m_integer m);
/* Read and write primitives */

#endif /* _MINTEGER_H_ */

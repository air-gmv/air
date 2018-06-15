#include <stdlib.h>
#include <pthread.h>

#include "minteger.h"
#include "utils.h"

/*****************************************************************************/
m_integer MI_init(int priority) {
  m_integer m;
  m = (m_integer) malloc(sizeof(struct s_m_integer));
  (*m).value = 0;
  pthread_mutexattr_t my_attr;
  CHECK_NZ (pthread_mutexattr_init (&my_attr));
  //  CHECK_NZ (pthread_mutexattr_setprotocol (&my_attr, PTHREAD_PRIO_PROTECT));
  //  CHECK_NZ (pthread_mutexattr_setprioceiling (&my_attr, priority));
  CHECK_NZ (pthread_mutex_init(&((*m).mutex),&my_attr));
  return m;
}

/*****************************************************************************/
void MI_write(m_integer m, int v) {
  CHECK_NZ (pthread_mutex_lock (&((*m).mutex)));     /* Enter critical section */
  (*m).value = v;
  CHECK_NZ (pthread_mutex_unlock (&(*m).mutex));   /* Leave critical section */
}

/*****************************************************************************/
int MI_read(m_integer m) {
  int v;

  CHECK_NZ (pthread_mutex_lock (&(*m).mutex));     /* Enter critical section */
  v=(*m).value;
  CHECK_NZ (pthread_mutex_unlock (&(*m).mutex));   /* Leave critical section */
  return v;
}

/*****************************************************************************/
#ifdef __TEST__
int main (void) {
  m_integer my_minteger = MI_init (150);
  MI_write (my_minteger, 42);
  printf ("Read %d\n", MI_read (my_minteger));
  return 0;
}
#endif /* __TEST__ */

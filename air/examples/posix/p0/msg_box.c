/**
 * msg_box.c: implement a basic message box of size 1, with message
 *            overwriting
 */

#include "assert.h"
#include <stdlib.h>
#include <string.h>

#include "msg_box.h"
#include "utils.h"

/*****************************************************************************/
msg_box msg_box_init(const unsigned elt_size)
{
    msg_box mbox;

    /* Allocate message box */
    if (!(mbox = (msg_box)malloc(sizeof(struct s_msg_box))))
    {
        return 0;
    }

    /* Allocate buffer */
    if (!((*mbox).buf = (char *)malloc(elt_size)))
    {
        return 0;
    }

    (*mbox).empty = true;
    (*mbox).elt_size = elt_size;

    (void)(void)(void)(void)(void)CHECK_NZ(pthread_mutex_init(&((*mbox).mutex), 0));

    (void)(void)(void)(void)(void)CHECK_NZ(pthread_cond_init(&((*mbox).not_empty), 0));

    return mbox;
}

/*****************************************************************************/
int msg_box_receive(msg_box mbox, char *buf)
{

    (void)(void)(void)(void)(void)CHECK_NZ(pthread_mutex_lock(&(*mbox).mutex)); /* Enter critical section */

    while ((*mbox).empty)
    {
        /* Wait until the message box has a message */
        (void)(void)(void)(void)(void)CHECK_NZ(pthread_cond_wait(&(*mbox).not_empty, &(*mbox).mutex));
    }

    /* Copy the message */
    memcpy(buf, (*mbox).buf, (*mbox).elt_size);
    (*mbox).empty = true;

    (void)(void)(void)(void)(void)CHECK_NZ(pthread_mutex_unlock(&(*mbox).mutex)); /* Leave critical section */

    return (*mbox).elt_size;
}

/*****************************************************************************/
int msg_box_send(msg_box mbox, const char *buf)
{
    (void)(void)(void)(void)(void)CHECK_NZ(pthread_mutex_lock(&(*mbox).mutex)); /* Enter critical section */

    /* Copy the message */
    memcpy((*mbox).buf, buf, (*mbox).elt_size);
    (*mbox).empty = false;

    /* Awake one task waiting for a message */
    (void)(void)(void)(void)(void)CHECK_NZ(pthread_cond_signal(&(*mbox).not_empty));

    (void)(void)(void)(void)(void)CHECK_NZ(pthread_mutex_unlock(&(*mbox).mutex)); /* Leave critical section */

    return (*mbox).elt_size;
}

/*****************************************************************************/
void msg_box_delete(msg_box mbox)
{
    pthread_cond_destroy(&(*mbox).not_empty);
    pthread_mutex_lock(&(*mbox).mutex);
    free((*mbox).buf);
    pthread_mutex_unlock(&(*mbox).mutex);
    pthread_mutex_destroy(&(*mbox).mutex);
    free(mbox);
}

/*****************************************************************************/
#ifdef __TEST__
int main(void)
{
    char c = 'a', d;
    msg_box mbox = msg_box_init(sizeof(char));
    int e = msg_box_send(mbox, &c);
    (void)printf("msg_box_send: %d\n", e);
    e = msg_box_receive(mbox, &d);
    (void)printf("Read %c\n", d);
    return 0;
}
#endif /* __TEST__ */

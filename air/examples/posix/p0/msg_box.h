/**
 * msg_box.h: define a basic message box of size 1, with message
 *            overwriting
*/

#ifndef _MSG_BOX_H_
#define _MSG_BOX_H_

#include <pthread.h>
#include <stdbool.h>

typedef struct s_msg_box {
  char *buf;        /* Buffer allocated by msb_box_init to store *1* message */
  bool empty;       /* State variable, true iff msg_box is empty */
  pthread_mutex_t mutex;
  pthread_cond_t not_empty; /* Awake a task blocked on the message box
                               when a message arrives */
  unsigned elt_size;/* Size of a message in the message box */
} *msg_box;

msg_box msg_box_init(const unsigned elt_size);
/** Create a message box to store one element of size 'elt_size' bytes
 *  - precondition: elt_size > 0
 *  - postcodition: the message box if success, NULL otherwise
 */

int msg_box_receive(msg_box mbox, char *buf);
/** Wait for an incoming message. This is a blocking call
 *  - precondition: mbox initialized, sizeof (buf) >= message size
 *  - postcondition: buf stores received message, return the size of
 *    the read message
 */

int msg_box_send(msg_box mbox, const char *buf);
/** Send a message to message box mbox, eventually overwrites any
 *  previously written message. This is a non blocking primitive.
 *  - precondition: mbox initialized,  sizeof (buf) >= message size
 *  - postcondition: mbox stores received message, return the size of
 *    the written message
 */

void msg_box_delete(msg_box mbox);
/** Delete message box mbox,
 *  - precondition: mbox initialized,
 *  - postcondition: internal elements of mbox are deallocated
 */

#endif /* _MSG_BOX_H_ */

/* based on a example-code from Keil for CS G++ */

/* for caddr_t (typedef char * caddr_t;) */
#include <sys/types.h>

extern int  __heap_start__;

caddr_t _sbrk ( int incr ) {

    static unsigned char *heap = NULL;
    unsigned char *prev_heap;

    if (heap == NULL) {
        heap = (unsigned char *)&__heap_start__;
    }

    prev_heap = heap;
    /* check removed to show basic approach */

    heap += incr;

    return (caddr_t) prev_heap;
}

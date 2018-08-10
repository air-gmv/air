/* User code: This file will not be overwritten by TASTE. */

#include "hello.h"
#include <stdio.h>

void hello_startup()
{
    /* Write your initialization code here,
       but do not make any call to a required interface. */
    printf("hello_startup()\n");
}

void hello_PI_s(const asn1SccT_Boolean *IN_t)
{
    /* Write your code here! */
    printf("hello_PI_s() %d\n", *IN_t);
}


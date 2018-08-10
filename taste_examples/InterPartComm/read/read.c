/* User code: This file will not be overwritten by TASTE. */

#include "read.h"
#include <stdio.h>

void read_startup()
{
    /* Write your initialization code here,
       but do not make any call to a required interface. */
    printf("read_startup()\n");
}

void read_PI_h()
{
    /* Write your code here! */
    printf("read_PI_h()\n");
    asn1SccT_Boolean a = 1;
    read_RI_s(&a);
}


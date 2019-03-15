#include <rtems.h>

uint8_t chksum8(const unsigned char *buff, size_t len)
{
    unsigned int sum;       // nothing gained in using smaller types!
    for ( sum = 0 ; len != 0 ; len-- )
        sum += *(buff++);   // parenthesis not required!
    return (uint8_t)sum;
}

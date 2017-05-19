
#include <stdarg.h>
#include <libtest.h>

/**
 * @brief Current partition buffer area (defined in libtest.c)
 */
extern partition_buffer_t *partition_buffer;

/**
 * @brief Output character into the partition internal buffer
 * @param ch input character
 */
static void partition_output_char(char ch) {

    /* sanity check */
    if (NULL == partition_buffer) {
        return;
    }

    /* write char */
    partition_buffer->buffer[partition_buffer->w_pos] = ch;

    /* increment the position on the buffer */
    partition_buffer->w_pos = INCREMENT_POSITION(partition_buffer->w_pos);

    /* mark a end of line */
    if ('\n' == ch) {

        partition_buffer->l_pos = partition_buffer->w_pos;
    }
}


static void vprintf_num(
        long unsigned int num,
        int base,
        int sign,
        int maxwidth,
        int lead) {

    long unsigned int n;
    int count;
    char toPrint[20];

    /* check the sign */
    if(( sign == 1 ) && ( (long) num < 0 ))

    {
        /* number is negative, print "-" */
        partition_output_char( '-');
        num = -num;
        if(maxwidth) maxwidth--;
    }

    count = 0;
    while(( n = num / base ) > 0)
    {
        toPrint[count++] = ( num - ( n * base ) );
        num = n;
    }
    toPrint[count++] = num;

    for(n = maxwidth; n > count; n--)
        partition_output_char( lead);

    /* output the characters */
    for(n = 0; n < count; n++) {
        partition_output_char("0123456789ABCDEF"[(int)(toPrint[count - (n + 1)])]);
    }
}


static void vprintf(const char *fmt , va_list ap) {

    char c , *str;
    int lflag , base , sign , width , lead;

    /* parse the input format */
    for(; *fmt != '\0'; fmt++) {

        /* escape character found */
        if(*fmt == '%') {

            /* initialize the formation flags */
            lflag = 0;
            base = 0;
            sign = 0;
            width = 0;
            lead = ' ';

            /* skip the escape character */
            fmt++;

            /* check for the leading character format */
            if(*fmt == '0') {

                lead = '0';
                fmt++;
            }

            /* check the size of the format filed */
            while(*fmt >= '0' && *fmt <= '9')
            {
                width *= 10;
                width += (*fmt - '0');
                fmt++;
            }

            /* check if it an long number */
            if((c = *fmt) == 'l') {

                lflag = 1;
                c = *++fmt;
            }

            /* check the special character */
            switch(c)
            {
                /* use base 8 */
                case 'o': case 'O': base = 8;
                    sign = 0;
                    break;

                /* use base 10 */
                case 'i': case 'I':
                case 'd': case 'D': base = 10;
                    sign = 1;
                    break;

                /* use base 10 */
                case 'u': case 'U': base = 10;
                    sign = 0;
                    break;

                    /* use base 16 */
                case 'x': case 'X': base = 16;
                    sign = 0;
                    break;

                /* use base 16 */
                case 'p': base = 16;
                    sign = 0;
                    break;

                /* use string */
                case 's':

                    for(str = va_arg(ap , char *); *str; str++) {
                        partition_output_char(*str);
                    }
                    break;

                /* use char */
                case 'c':

                    partition_output_char(va_arg(ap , int));
                    break;

                /* use char */
                default:
                    partition_output_char(c);
                    break;

            }

            /* out number if the base is defined */
            if(base)
            {
                vprintf_num(lflag ? va_arg(ap , long int) :
                        (long int) va_arg(ap , int) , base , sign , width , lead);
            }

        } else {

            /* ouput the character */
            partition_output_char(*fmt);
        }
    }
}

/**
 * @brief Test printf
 * @param fmt string format
 * @param ... input arguments
 */
void pprintf(const char *fmt , ...) {

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}


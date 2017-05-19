/**
 * @file
 * @author pfnf
 * @brief Contains function to print messages exclusively from the PMK
 */

#include <bsp.h>
#include <printk.h>
#include <stdarg.h>

/**
 * @brief Printk formation flags
 * @ingroup pmk_printk
 */
typedef enum {

    ZEROPAD  = 0x01,    /**< zero padding                   */
    POINTER  = 0x02,    /**< number is a pointer            */
    SIGNED   = 0x04,    /**< number is signed               */
    SHORT    = 0x08,    /**< number is short                */
    LONG     = 0x10,    /**< number is long                 */

} format_flags_e;

/**
 * @brief Print a number to the serial output
 * @param number Number to print
 * @param base Base of number to print
 * @param width Width of the number to print
 * @param flags Formation flags for the number
 * @ingroup pmk_printk
 */
static void print_number(
    unsigned long number, int base, int width, format_flags_e flags) {

    static const char digits[16] = "0123456789ABCDEF";

    /* check the sign */
    if ((flags & SIGNED) != 0 && ((long)number < 0)) {
        pmk_console_outbyte('-');
        if (width > 0) --width;
        number = -number;
    }

    /* decompose number */
    int count = 0;
    char buffer[24];
    long unsigned i;
    while ((i = number / base) > 0) {
        buffer[count++] = (number - (i * base));
        number = i;
    }
    buffer[count++] = number;

    /* apply padding */
    char lead = ' ';
    if ((flags & ZEROPAD) != 0) lead = '0';
    for (i = width; i > count; --i) {
        pmk_console_outbyte(lead);
    }

    /* output number */
    for(i = 0; i < count; i++) {
        pmk_console_outbyte(digits[(int)(buffer[count - (i + 1 )])]);
    }
}

/**
 * @brief Prints a message base on its formation string and input arguments
 * @param fmt Formation string
 * @param args Input arguments
 * @ingroup pmk_printk
 */
static void vprintk(const char *fmt, va_list args) {

    char *str;
    int width, base;
    unsigned long number;
    format_flags_e flags;

    /* process complete format string */
    while (*fmt != '\0') {

        /* process special character */
        if (*fmt == '%') {

            ++fmt;

            /* clear format flags */
            flags = width = base = 0;

            /* check for zero pad */
            if (*fmt == '0') {
                flags |= ZEROPAD;
                ++fmt;
            }

            /* get format width */
            while (*fmt >= '0' && *fmt <= '9') {
                width = (width * 10) + (*fmt - '0');
                ++fmt;
            }

            /* check for short modifier */
            if (*fmt == 'h') {
                flags |= SHORT;
                ++fmt;
            }

            /* check for long modifier */
            if (*fmt == 'l') {
                flags |= LONG;
                ++fmt;
            }

            /* modifier */
            switch (*fmt) {

                /* signed number */
                case 'i': case 'd': case 'I': case 'D':
                     flags |= SIGNED;
                     base = 10;
                     break;

                /* pointer */
                case 'p': case 'P':
                     flags |= (POINTER | ZEROPAD);
                     width = 2 * sizeof(void *);
                     pmk_console_outbyte('0');
                     pmk_console_outbyte('x');
                     base = 16;
                     break;

                /* hexadecimal */
                case 'x': case 'X':
                     base = 16;
                     break;

                /* string */
                case 's':
                    for (str = va_arg(args , char *); *str != '\0'; ++str) {
                        pmk_console_outbyte(*str);
                    }
                    break;

                /* character */
                case 'c':
                     pmk_console_outbyte((char)va_arg(args , int));
                     break;

                /* give up and print format string */
                default:
                     pmk_console_outbyte(*fmt);
                     break;
            }

            /* print number, if base got specified */
            if (base > 0) {

                /* get number, base on the formation flags */
                if ((flags & POINTER) != 0) {
                    number = (unsigned long)va_arg(args, void *);
                } else if ((flags & LONG) != 0) {
                    number = va_arg(args, unsigned long);
                } else if ((flags & SHORT) != 0 && (flags & SIGNED) == 0) {
                    number = (unsigned short)va_arg(args, int);
                } else if ((flags & SHORT) != 0 && (flags & SIGNED) != 0) {
                    number = (short)va_arg(args, int);
                } else if ((flags & SIGNED) == 0) {
                    number = va_arg(args, unsigned int);
                } else {
                    number = va_arg(args, int);
                }
 
                print_number(number, base, width, flags);
            }

        /* output character */
        } else {
            pmk_console_outbyte(*fmt);
        }

        /* advance in the format string */
        ++fmt;
    }
}

void printk(const char *fmt , ...){

    va_list ap;
    va_start(ap , fmt);
    vprintk(fmt , ap);
    va_end(ap);
}

void pmk_console_outbyte(char ch) {

    /* add \r before \n to prevent display errors */
    if (ch == '\n') {
        bsp_console_outbyte('\r');
    }

    /* send byte to BSP console */
    bsp_console_outbyte(ch);
} 

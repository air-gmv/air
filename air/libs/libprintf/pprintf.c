/**
 * @file
 * @author pfnf
 * @brief Simplified partition printf
 */

#include <air.h>
#include <pprintf.h>

/**
 * @brief pprintf buffer size
 * @ingroup lib_pprintf
 */
#define BUFFER_SIZE                                         (64)

/**
 * @brief pprintf formation flags
 * @ingroup lib_pprintf
 */
typedef enum {

    ZEROPAD  = 0x01,            /**< zero padding                   */
    POINTER  = 0x02,            /**< number is a pointer            */
    SIGNED   = 0x04,            /**< number is signed               */
    SHORT    = 0x08,            /**< number is short                */
    LONG     = 0x10,            /**< number is long                 */

} format_flags_e;

/**
 * @brief pprintf library current partition id
 * @ingroup lib_pprintf
 */
static air_identifier_t partition_idx = 0;

/**
 * @brief pprintf library initialization flag
 * @ingroup lib_pprintf
 */
static air_boolean printf_inited = 0;

/**
 * @brief pprintf partition color table
 * @ingroup lib_pprintf
 */
static char partition_colors[][6] = {
    "\x1b[33m\0",       /**< color yellow   */
    "\x1b[32m\0",       /**< color green    */
    "\x1b[31m\0",       /**< color red      */
    "\x1b[34m\0",       /**< color blue     */
    "\x1b[36m\0",       /**< color cyan     */
    "\x1b[35m\0",       /**< color magenta  */
};

/**
 * @brief Initializes the partition porintf library support
 * @ingroup lib_pprintf
 */
static void printf_init() {

    /* initialize pprintf support */
    if (printf_inited == 0) {

        /* get partition index */
        air_partition_status_t p_status;
        if (air_syscall_get_partition_status(-1, &p_status) != AIR_NO_ERROR) {
            return;
        }

        /* store partition index */
        partition_idx = p_status.index;

        /* flag lib as initiated */
        printf_inited = 1;
    }

    /* print color change string */
    air_syscall_print(partition_colors[partition_idx % 6], 6);
}

/**
 * @brief Print byte
 * @param ch byte to print
 */
static void print_byte(char *print_buffer, int *buffer_idx, char ch) {

    print_buffer[*buffer_idx] = ch;
    ++(*buffer_idx);

    /* flush buffer */
    if (*buffer_idx >= BUFFER_SIZE) {

        air_syscall_print(print_buffer, BUFFER_SIZE);
        *buffer_idx = 0;
    }
}

/**
 * @brief Print an integer number
 * @param number Number to print
 * @param base Base of number to print
 * @param width Width of the number to print
 * @param flags Formation flags for the number
 * @ingroup pmk_printk
 */
static void print_number(
        char *print_buffer, int *buffer_idx,
        unsigned long number, int base, int width, format_flags_e flags) {

    static const char digits[16] = "0123456789ABCDEF";

    /* check the sign */
    if ((flags & SIGNED) != 0 && ((long)number < 0)) {
        print_byte(print_buffer, buffer_idx, '-');
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
        print_byte(print_buffer, buffer_idx, lead);
    }

    /* output number */
    for(i = 0; i < count; i++) {
        print_byte(
                print_buffer, buffer_idx,
                digits[(int)(buffer[count - (i + 1 )])]);
    }
}

/**
 * @brief Prints a message base on its formation string and input arguments
 * @param fmt Formation string
 * @param args Input arguments
 * @ingroup pmk_printk
 */
static void vpprintf(const char *fmt, va_list ap) {

    char *str;
    int width, base;
    unsigned long number;
    format_flags_e flags;

    /* internal pprintf buffer */
    int buffer_idx = 0;
    char buffer[BUFFER_SIZE];

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
                    print_byte(buffer, &buffer_idx, '0');
                    print_byte(buffer, &buffer_idx, 'x');
                    base = 16;
                    break;

                /* hexadecimal */
                case 'x': case 'X':
                    base = 16;
                    break;

                /* string */
                case 's':
                    for (str = va_arg(ap , char *); *str != '\0'; ++str) {
                        print_byte(buffer, &buffer_idx, *str);
                    }
                    break;

                /* character */
                case 'c':
                    print_byte(buffer, &buffer_idx, va_arg(ap , int));
                    break;

                /* give up and print format string */
                default:
                    print_byte(buffer, &buffer_idx, *fmt);
                    break;
            }

            /* print number, if base got specified */
            if (base > 0) {

                /* get number, base on the formation flags */
                if ((flags & POINTER) != 0) {
                    number = (unsigned long)va_arg(ap, void *);
                } else if ((flags & LONG) != 0) {
                    number = va_arg(ap, unsigned long);
                } else if ((flags & SHORT) != 0 && (flags & SIGNED) == 0) {
                    number = (unsigned short)va_arg(ap, int);
                } else if ((flags & SHORT) != 0 && (flags & SIGNED) != 0) {
                    number = (short)va_arg(ap, int);
                } else if ((flags & SIGNED) == 0) {
                    number = va_arg(ap, unsigned int);
                } else {
                    number = va_arg(ap, int);
                }

                print_number(buffer, &buffer_idx, number, base, width, flags);
            }

        /* output character */
        } else {
            print_byte(buffer, &buffer_idx, *fmt);
        }

        /* advance in the format string */
        ++fmt;
    }

    /* print the remaind of the buffer */
    if (buffer_idx > 0) {

        buffer[buffer_idx] = '\0';
        air_syscall_print(buffer, buffer_idx + 1);
    }
}

void libpprintf_internal_pprintf(const char *fmt , ...) {

    /* initialize the console */
    printf_init();

    /* format and print string */
    va_list ap;
    va_start(ap , fmt);
    vpprintf(fmt , ap);
    va_end(ap);
}

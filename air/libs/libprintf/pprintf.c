/*
 * Copyright (C) 2008-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf, bmmd
 * @brief Simplified partition printf
 */
#include <air.h>
#include <pprintf.h>

/**
 * @brief pprintf buffer size
 * @ingroup lib_pprintf
 */
#define BUFFER_SIZE (64)

/**
 * @brief pprintf formation flags
 * @ingroup lib_pprintf
 */
typedef enum
{

    ZEROPAD = 0x01, /**< zero padding                   */
    POINTER = 0x02, /**< number is a pointer            */
    SIGNED = 0x04,  /**< number is signed               */
    SHORT = 0x08,   /**< number is short                */
    LONG = 0x10,    /**< number is long                 */

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
static air_boolean_t printf_inited = false;

/**
 * @brief pprintf partition color table
 * @ingroup lib_pprintf
 */
static char partition_colors[][6] = {
    COLOR_KYELLOW,  /**< color yellow   */
    COLOR_KGREEN,   /**< color green    */
    COLOR_KRED,     /**< color red      */
    COLOR_KBLUE,    /**< color blue     */
    COLOR_KCYAN,    /**< color cyan     */
    COLOR_KMAGENTA, /**< color magenta  */
};

/**
 * @brief Initializes the partition porintf library support
 * @ingroup lib_pprintf
 */
static void printf_init()
{

    /* initialize pprintf support */
    if (printf_inited == 0)
    {

        /* get partition index */
        air_partition_status_t p_status;
        if (air_syscall_get_partition_status(-1, &p_status) != AIR_NO_ERROR)
        {
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
static void print_byte(char *print_buffer, int *buffer_idx, char ch)
{

    print_buffer[*buffer_idx] = ch;
    ++(*buffer_idx);

    /* flush buffer */
    if (*buffer_idx >= BUFFER_SIZE)
    {

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
static void print_number(char *print_buffer, int *buffer_idx, unsigned long number, int base, int width,
                         format_flags_e flags)
{

    static const char digits[16] = "0123456789ABCDEF";

    /* check the sign */
    if ((flags & SIGNED) != 0 && ((long)number < 0))
    {
        print_byte(print_buffer, buffer_idx, '-');
        if (width > 0)
        {
            --width;
        }

        number = -number;
    }

    /* decompose number */
    int count = 0;
    char buffer[24];
    long unsigned i;
    while ((i = number / base) > 0)
    {
        buffer[count++] = (number - (i * base));
        number = i;
    }
    buffer[count++] = number;

    /* apply padding */
    char lead = ' ';
    if ((flags & ZEROPAD) != 0)
    {
        lead = '0';
    }

    for (i = width; i > count; --i)
    {
        print_byte(print_buffer, buffer_idx, lead);
    }

    /* output number */
    for (i = 0; i < count; i++)
    {
        print_byte(print_buffer, buffer_idx, digits[(int)(buffer[count - (i + 1)])]);
    }
}

/**
 * @brief Print a float/double number
 * @param value float/double number to print
 * @param width Width of the number to print
 * @param precision Precision of the number to print (in other words, number of decimal digits)
 * @param flags Formation flags for the number
 * @ingroup pmk_printk
 */
static void print_float(char *print_buffer, int *buffer_idx, double value, int width, int precision,
                         format_flags_e flags)
{   
    air_i64_t int_part = (air_i64_t)value; // integer part of a double can be as big as 53 bit (which translates to 16 digits max)
    char reversed_int_buffer[20];
    char buffer[36];
    double frac_part = value - int_part;
    int count = 0; // count number of digits to be printed
    int i = 0; // auxiliary variable used for iteration in both integer and decimal parts of the value
    //air_i64_t i = 0;
    air_i64_t aux_digit = 0;
    air_i64_t frac_as_int;
    double rounding_factor = 1;

    /* define padding */
    char lead = ' ';
    if ((flags & ZEROPAD) != 0)
    {
        lead = '0';
    }

    // handle sign
    if (value < 0)
    {
        if(lead == '0') // minus sign appears before the leading zeros
        {
            print_byte(print_buffer, buffer_idx, '-');
            width--;
        }
        else // minus sign appears after the leading spaces, so should be included in the buffer
        {
            buffer[count++] = '-';
        }
        
        int_part = -int_part;
        frac_part = -frac_part;
    }

    // Convert fractional part to integer for rounding
    // *** rounding_factor = pow(10, precision); ***
    // Changed to avoid using math.h, so it's compatible 
    // with both BARE and RTMES5 personalities
    for (i=0; i<precision; i++)
    {
        rounding_factor *= 10;
    }
    frac_as_int = (air_i64_t)(frac_part * rounding_factor + 0.5);

    // Check if rounding affects the integer part
    if (frac_as_int >= rounding_factor) 
    {
        // move value from fractional part
        // to the integer part
        frac_as_int -= rounding_factor;
        int_part += 1;
    }

    // handle integer part
    // case zero, can't be handled by the "normal" routine for getting the integer digits
    if (int_part == 0) 
    {
        buffer[count++] = '0';
    } 
    else
    {
        // get integer digits (reversed) and count
        while (int_part > 0) 
        {
            reversed_int_buffer[count++] = (int_part % 10) + '0';
            int_part /= 10;
        }
        // fill the main buffer with digits in the correct order
        for ( i = 0 ; i < count ; i++ )
        {
            buffer[i] = reversed_int_buffer[count - i - 1];
        }

    }

    // handle decimal part
    if (precision > 0)
    {
        buffer[count++] = '.';
        count += precision; // update count with number of decimal digits (precision)
        // fill the buffer backwards, from least significant digit to most significant digit
        for (i = 0; i < precision; i++)
        {
            aux_digit = frac_as_int % 10;
            buffer[count - 1 - i] = aux_digit + '0';
            frac_as_int /= 10;
        }
        

    }

    // handle width
    for (i = width; i > count; --i)
    {
        print_byte(print_buffer, buffer_idx, lead);
    }

    // print buffer
    for (i = 0; i < count; i++)
    {
        print_byte(print_buffer, buffer_idx, buffer[i]);
    }

}

/**
 * @brief Prints a message base on its formation string and input arguments
 * @param fmt Formation string
 * @param args Input arguments
 * @ingroup pmk_printk
 */
static void vpprintf(const char *fmt, va_list ap)
{

    char *str;
    int width, base;
    unsigned long number;
    format_flags_e flags;
    int float_precision = 6; // default for floats

    /* internal pprintf buffer */
    int buffer_idx = 0;
    char buffer[BUFFER_SIZE];

    /* process complete format string */
    while (*fmt != '\0')
    {

        /* process special character */
        if (*fmt == '%')
        {

            ++fmt;

            /* clear format flags */
            flags = width = base = 0;

            /* check for zero pad */
            if (*fmt == '0')
            {
                flags |= ZEROPAD;
                ++fmt;
            }

            /* get format width */
            while (*fmt >= '0' && *fmt <= '9')
            {
                width = (width * 10) + (*fmt - '0');
                ++fmt;
            }

            // decimal places format (supported by floats and doubles)
            if (*fmt == '.') {
                fmt++;
                float_precision = 0;
                while (*fmt >= '0' && *fmt <= '9') {
                    float_precision = float_precision * 10 + (*fmt - '0');
                    fmt++;
                }
            }

            /* check for short modifier */
            if (*fmt == 'h')
            {
                flags |= SHORT;
                ++fmt;
            }

            /* check for long modifier */
            if (*fmt == 'l')
            {
                flags |= LONG;
                ++fmt;
            }

            /* modifier */
            switch (*fmt)
            {

            /* signed number */
            case 'i':
            case 'd':
            case 'I':
            case 'D':
                flags |= SIGNED;
                base = 10;
                break;
            case 'f':
                flags |= SIGNED;
                print_float(buffer, &buffer_idx, va_arg(ap, double), width, float_precision, flags);
                break;
            /* pointer */
            case 'p':
            case 'P':
                flags |= (POINTER | ZEROPAD);
                width = 2 * sizeof(void *);
                print_byte(buffer, &buffer_idx, '0');
                print_byte(buffer, &buffer_idx, 'x');
                base = 16;
                break;

            /* hexadecimal */
            case 'x':
            case 'X':
                base = 16;
                break;

            /* string */
            case 's':
                for (str = va_arg(ap, char *); *str != '\0'; ++str)
                {
                    print_byte(buffer, &buffer_idx, *str);
                }
                break;

            /* character */
            case 'c':
                print_byte(buffer, &buffer_idx, va_arg(ap, int));
                break;

            /* give up and print format string */
            default:
                print_byte(buffer, &buffer_idx, *fmt);
                break;
            }

            /* print number, if base got specified */
            if (base > 0)
            {

                /* get number, base on the formation flags */
                if ((flags & POINTER) != 0)
                {
                    number = (unsigned long)va_arg(ap, void *);
                }
                else if ((flags & LONG) != 0)
                {
                    number = va_arg(ap, unsigned long);
                }
                else if ((flags & SHORT) != 0 && (flags & SIGNED) == 0)
                {
                    number = (unsigned short)va_arg(ap, int);
                }
                else if ((flags & SHORT) != 0 && (flags & SIGNED) != 0)
                {
                    number = (short)va_arg(ap, int);
                }
                else if ((flags & SIGNED) == 0)
                {
                    number = va_arg(ap, unsigned int);
                }
                else
                {
                    number = va_arg(ap, int);
                }

                print_number(buffer, &buffer_idx, number, base, width, flags);
            }

            /* output character */
        }
        else
        {
            print_byte(buffer, &buffer_idx, *fmt);
        }

        /* advance in the format string */
        ++fmt;
    }

    /* print the remaind of the buffer */
    if (buffer_idx > 0)
    {

        buffer[buffer_idx] = '\0';
        air_syscall_print(buffer, buffer_idx + 1);
    }
}

void libpprintf_internal_pprintf(const char *fmt, ...)
{

    /* initialize the console */
    printf_init();

    /* format and print string */
    va_list ap;
    va_start(ap, fmt);
    vpprintf(fmt, ap);
    va_end(ap);
}

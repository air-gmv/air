/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Hello! Is it me you're looking for?
 */

#include <air.h>
#include <math.h>
#include "pprintf.h"


/**
 * @brief Partition entry point
 */
void entry_point()
{

    //air_syscall_print(message, sizeof(message));

    float pi =  3.141592653589793238462643383279502884197;
    pprintf("default float:\n");
    pprintf("\tpi = %f\n", pi);
    pprintf("The \"0.1 + 0.2\" case, with 16 decimal places:\n");
    float value = 0.1 + 0.2;
    pprintf("\t%.16f\n", value);
    pprintf("2.999999999 with two decimal places:\n");
    pprintf("\t%.2f\n", 2.999999999);
    // NOTE: the width includes the '.'
    pprintf("Float with 6 width and 2 decimal places:\n");
    pprintf("\tpi = %6.2f\n", pi);
    pprintf("Float with 6 width with 0 padding and 2 decimal places:\n");
    pprintf("\tpi = %06.2f\n", pi);
    pprintf("Float zero:\n");
    pprintf("\t%f\n", 0.0);
    pprintf("Float zero with 7 width, zero padding and 3 decimal places:\n");
    pprintf("\t%07.3f\n", 0.0);
    pprintf("Negative Float 1:\n");
    pprintf("\t-pi = %f\n", -pi);
    pprintf("Negative Float 2:\n");
    pprintf("\t%f\n", -0.123);
    pprintf("Float with 0 width and 3 decimal places (width should be ignored):\n");
    pprintf("\tpi = %0.3f\n", pi);
    pprintf("Float with 0 decimal places:\n");
    pprintf("\tpi = %.0f\n", pi);


    air_syscall_shutdown_module();

}

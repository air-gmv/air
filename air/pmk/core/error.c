/*
 * Copyright (C) 2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief PMK internal error handling functions
 */

#include <error.h>
#include <multicore.h>
#include <pmk.h>
#include <printk.h>

void pmk_fatal_error(pmk_error_e code, const char *func, const char *file, int line)
{

    /* print error message */
    printk(" :: error %i, function '%s' in file '%s' at line %i\n", code, func, file, line);

    /* shutdown module */
    pmk_core_ctrl_t *core = pmk_get_current_core_ctrl();
    pmk_module_shutdown(core);
}

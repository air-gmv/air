/*
 * Copyright (C) 2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief PMK internal error handling functions
 */

#include <pmk.h>
#include <error.h>
#include <printk.h>
#include <multicore.h>

void pmk_fatal_error(
        pmk_error_e code, const char *func, const char *file, int line) {

    /* print error message */
    printk(" :: error %i, function '%s' in file '%s' at line %i\n",
            code, func, file, line);

    /* shutdown module */
    pmk_core_ctrl_t *core = pmk_get_current_core_ctrl();
    pmk_module_shutdown(core);
}


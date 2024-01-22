/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file configurations.c
 * @author pfnf
 * @brief Functions (geters) to access the application configurations
 */

#include <pmk.h>
#include <error.h>
#include <printk.h>
#include <configurations.h>

/**
 * @brief Beginning of the application configuration (from link commands)
 * @ingroup pmk_configuration
 */
extern air_u32_t air_configuration;

/**
 * @brief Pointer to the application configuration
 */
pmk_configuration_t *pmk_configuration;

void pmk_configurations_int(void)
{

    /* get pointer to module configuration */
    pmk_configuration = (pmk_configuration_t *)(&air_configuration);

#ifdef PMK_DEBUG
    printk(" :: Configuration initialization (%p)\n", pmk_configuration);
#endif

    /* check if configuration is valid */
    if (pmk_configuration->magic != PMK_CONFIG_MAGIC_NUMBER)
    {
        pmk_fatal_error(PMK_INTERNAL_ERROR_CONFIG, __func__, __FILE__, __LINE__);
    }
    return;
}

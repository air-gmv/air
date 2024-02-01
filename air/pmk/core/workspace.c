/*
 * Copyright (C) 2013-2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  @file
 *  @author pfnf
 * @brief Contains the main control structures for the PMK and the internal
 *        memory allocation primitives
 */

#include <pmk.h>
#include <error.h>
#include <printk.h>
#include <workspace.h>
#include <segregation.h>

/**
 * @brief Current workspace pointer
 * @ingroup pmk_workspace
 */
static air_uptr_t current_pointer;
/**
 * @brief Initial workspace memory area
 * @ingroup pmk_workspace
 */
extern air_u32_t air_workspace;
/**
 * @brief End of the workspace memory area
 * @ingroup pmk_workspace
 */
extern air_u32_t air_kernel_memory_end;

/**
 * @brief PMK Shared area
 */
pmk_sharedarea_t air_shared_area;

/**
 * @brief Workspace size
 */
static air_sz_t workspace_size;

void *pmk_workspace_alloc(air_sz_t size)
{

    /* get a block of memory */
    void *block = (void *)ADDR_ALIGN(current_pointer, CPU_CRITICAL_ALIGN);
    current_pointer = (air_uptr_t)((air_sz_t)block + size);

    /* check if the memory is available*/
    air_sz_t used = (air_sz_t)current_pointer - (air_sz_t)&air_workspace;
    if (used > workspace_size)
    {
        printk(" %i - %i = - %i overhead\n", used, workspace_size, used - workspace_size);
        pmk_fatal_error(PMK_INTERNAL_ERROR_MEMORY, __func__, __FILE__, __LINE__);
    }

    return block;
}

void *pmk_workspace_aligned_alloc(air_sz_t size, air_u32_t align)
{

    /* get a block of memory */
    void *block = (void *)ADDR_ALIGN(current_pointer, align);
    current_pointer = (air_uptr_t)((air_sz_t)block + size);

    /* check if the memory is available*/
    air_sz_t used = (air_sz_t)current_pointer - (air_sz_t)&air_workspace;
    if (used > workspace_size)
    {
        printk(" %i - %i = - %i overhead\n", workspace_size, used, used - workspace_size);
        pmk_fatal_error(PMK_INTERNAL_ERROR_MEMORY, __func__, __FILE__, __LINE__);
    }

    return block;
}

void pmk_workspace_init(void)
{

#ifdef PMK_DEBUG
    printk(" :: Workspace initialization\n");
#endif

    /* setup first pointer */
    current_pointer = (air_uptr_t)&air_workspace;
    workspace_size = (air_sz_t)&air_kernel_memory_end - (air_sz_t)&air_workspace;

    /* clear air workspace */
    (void)memset(&air_workspace, 0x0, workspace_size);

#ifdef PMK_DEBUG
    printk("    initial memory block: 0x%08x\n", current_pointer);
#endif
    return;
}

#ifdef PMK_DEBUG
void pmk_workspace_debug(void)
{

    air_sz_t used = (air_sz_t)current_pointer - (air_sz_t)&air_workspace;
    printk("    workspace usage: %i of %i\n\n", used, workspace_size);
    return;
}
#endif

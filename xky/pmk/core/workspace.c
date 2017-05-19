/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
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
static xky_uptr_t current_pointer;
/**
 * @brief Initial workspace memory area
 * @ingroup pmk_workspace
 */
extern xky_u32_t xky_workspace;
/**
 * @brief End of the workspace memory area
 * @ingroup pmk_workspace
 */
extern xky_u32_t xky_kernel_memory_end;

/**
 * @brief PMK Shared area
 */
pmk_sharedarea_t xky_shared_area;


/**
 * @brief Workspace size
 */
static xky_sz_t workspace_size;

void *pmk_workspace_alloc(xky_sz_t size) {

    /* get a block of memory */
    void *block = (void *)ADDR_ALIGN(current_pointer, CPU_CRITICAL_ALIGN);
    current_pointer = (xky_uptr_t)block + size;

    /* check if the memory is available*/
    xky_sz_t used = (xky_sz_t)(current_pointer - (xky_uptr_t)&xky_workspace);
    if (used > workspace_size) {
        printk(" - %i overhead\n", used - workspace_size);
        pmk_fatal_error(PMK_INTERNAL_ERROR_MEMORY, __func__, __FILE__, __LINE__);
    }

    return block;
}

void *pmk_workspace_aligned_alloc(xky_sz_t size, xky_u32_t align) {

    /* get a block of memory */
    void *block = (void *)ADDR_ALIGN(current_pointer, align);
    current_pointer = (xky_uptr_t)block + size;

    /* check if the memory is available*/
    xky_sz_t used = (xky_sz_t)(current_pointer - (xky_uptr_t)&xky_workspace);
    if (used > workspace_size) {
        printk(" - %i overhead\n", used - workspace_size);
        pmk_fatal_error(PMK_INTERNAL_ERROR_MEMORY, __func__, __FILE__, __LINE__);
    }

    return block;
}

void pmk_workspace_init(void) {

#ifdef PMK_DEBUG
    printk(" :: Workspace initialization\n");
#endif

    /* clear shared area structure */
    memset(&xky_shared_area, 0x0, sizeof(pmk_sharedarea_t));

    /* setup first pointer */
    current_pointer = (xky_uptr_t)&xky_workspace;
    workspace_size = ((xky_sz_t)&xky_kernel_memory_end - (xky_sz_t)&xky_workspace);

#ifdef PMK_DEBUG
    printk("    initial memory block: 0x%08x\n", current_pointer);
#endif
    return;
}

#ifdef PMK_DEBUG
void pmk_workspace_debug(void) {

    xky_sz_t used = (xky_sz_t)(current_pointer - (xky_uptr_t)&xky_workspace);
    printk("    workspace usage: %i of %i\n\n", used, workspace_size);
    return;
}
#endif

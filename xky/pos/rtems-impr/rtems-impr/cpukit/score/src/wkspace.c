/**
 *  @file
 *  wkspace.c
 *
 *  @brief initialize the Workspace Handler
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreWorkspace Workspace Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/interr.h>


Heap_Control _Workspace_Area;


void _Workspace_Handler_initialization(
                                       void *starting_address ,
                                       size_t size
                                       )
{
    /* iterator for each entry of the table to zero out */
    uint32_t *zero_out_array;

    /* iterator index of the table to zero out */
    uint32_t index;

    /* memory available for the workspace */
    uint32_t memory_available;


    /* check the starting address */
    if(!starting_address || !_Addresses_Is_aligned(starting_address))
    {
        /* raise internal error if starting address is not valid */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_INVALID_WORKSPACE_ADDRESS);
    }

    /* check if the workspace should be initialized with 0's */
    if(_CPU_Table.do_zero_of_workspace)
    {
        /* then iterate through each address */
        for(zero_out_array = (uint32_t *) starting_address , index = 0;
            index < size / sizeof ( uint32_t );
            index++)
        {
            /* and place a 0 */
            zero_out_array[ index ] = 0;
        }
    }

    /* initialize the heap at the starting address */
    memory_available = _Heap_Initialize(&_Workspace_Area ,
                                        starting_address ,
                                        size ,
                                        CPU_HEAP_ALIGNMENT);

    /* if the heap could not be initialized */
    if(memory_available == 0)
    {
        /* then raise an internal error */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_TOO_LITTLE_WORKSPACE);
    }
}


void *_Workspace_Allocate_or_fatal_error(
                                         size_t size
                                         )
{
    /* allocated memory */
    void *memory;

    
    /* allocate the workspace memory */
    memory = _Workspace_Allocate(size);

    /* if the memory could not be allocated */
    if(memory == NULL)
    {
        /* then raise an internal error */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_WORKSPACE_ALLOCATION);
    }

    /* return the memory */
    return memory;
}

/**  
 *  @}
 */

/**
 *  @}
 */

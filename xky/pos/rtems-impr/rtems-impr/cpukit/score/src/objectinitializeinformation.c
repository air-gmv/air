/**
 *  @file
 *  objectinitializeinformation.c
 *
 *  @brief initialize the object information
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
 *  618     | 17/11/2008  | mcoutinho    | IPR 64
 *  3520    | 29/06/2009  | mcoutinho    | IPR 531
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5951    | 11/01/2010  | mcoutinho    | IPR 1048
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreObject Object Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
    #include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>


void _Objects_Initialize_information(
                                     Objects_Information *information ,
                                     Objects_APIs the_api ,
                                     uint16_t the_class ,
                                     uint32_t maximum ,
                                     uint16_t size ,
                                     boolean is_string ,
                                     uint32_t maximum_name_length
#if defined(RTEMS_MULTIPROCESSING)
    ,
                                     boolean supports_global ,
                                     Objects_Thread_queue_Extract_callout extract
#endif
    )
{
    /* NULL local table */
    static Objects_Control *null_local_table = NULL;

    /* minimum index for the object class */
    uint32_t minimum_index;

    /* number of bytes for the object name */
    uint32_t name_length;
#if defined(RTEMS_MULTIPROCESSING)

    /* index */
    uint32_t index;
#endif


    /* initialize the object table api */
    information->the_api = the_api;

    /* initialize the object table class */
    information->the_class = the_class;

    /* initialize the object table name is string */
    information->is_string = is_string;

    /* initialize the object table local object table */
    information->local_table = 0;

    /* initialize the object table local name table */
    information->name_table = 0;

    /* initialize the object table number of inactive objects */
    information->inactive_per_block = 0;

    /* initialize the object table number of inactive objects */
    information->object_blocks = 0;

    /* initialize the object table number of objects on the inactive list */
    information->inactive = 0;

    /* Set the entry in the object information table */
    _Objects_Information_table[ the_api ][ the_class ] = information;

    /* Set the size of the object */
    information->size = size;

    /* The allocation unit is the maximum value */
    information->allocation_size = maximum;

    /* Provide a null local table entry for the case of any empty table. */
    information->local_table = &null_local_table;

    /* calculate minimum and maximum Id's */

    /* calculate the minimum index */

    /* if the maximum is not set */
    if(maximum == 0)
    {
        /* then the minimum index is 0 */
        minimum_index = 0;
    }
    else
    {
        /* else, it is 1 */
        minimum_index = 1;
    }

    /* build the minimum identifer of the object class */
    information->minimum_id = _Objects_Build_id(the_api ,
                                                the_class ,
#if defined(RTEMS_MULTIPROCESSING)
        _Objects_Local_node ,
#endif
        minimum_index);

    /* calculate the maximum name length */

    /* initialize the name length with the maximum name */
    name_length = maximum_name_length;

    /* now align the name length to OBJECTS_NAME_ALIGNMENT */
    if(name_length & ( OBJECTS_NAME_ALIGNMENT - 1 ))
    {
        name_length = ( name_length + OBJECTS_NAME_ALIGNMENT ) &
            ~( OBJECTS_NAME_ALIGNMENT - 1 );
    }

    /* initialize the name length */
    information->name_length = name_length;

    /* initialize the object information table of the inactive nodes */
    _Chain_Initialize_empty(&information->Inactive);

    /* initialize objects .. if there are any */
    if(maximum)
    {
        /* reset the maximum value. It will be updated when the information is
         * extended */
        information->maximum = 0;

        /* always have the maximum size available so the current performance
         * figures are create are met.  If the user moves past the maximum
         * number then a performance hit is taken */

        _Objects_Extend_information(information);

    }

    /* take care of multiprocessing */

#if defined(RTEMS_MULTIPROCESSING)

    /* set the object extract function */
    information->extract = extract;

    /* if the object class can be global and if multiprocessing is enabled */
    if(supports_global == TRUE && _System_state_Is_multiprocessing)
    {
        /* allocate memory for the object global table */
        information->global_table =
            (Chain_Control *) _Workspace_Allocate_or_fatal_error(( _Objects_Maximum_nodes + 1 ) *
                                                                 sizeof (Chain_Control ));

        /* loop through the objects */
        for(index = 1; index <= _Objects_Maximum_nodes; index++)
        {
            /* and initialize each object global table chain */
            _Chain_Initialize_empty(&information->global_table[ index ]);
        }
    }
    else
    {
        information->global_table = NULL;
    }
#endif
}

/**  
 *  @}
 */

/**
 *  @}
 */

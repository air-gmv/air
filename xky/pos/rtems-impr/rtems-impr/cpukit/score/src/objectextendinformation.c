/**
 *  @file
 *  objectextendinformation.c
 *
 *  @brief extend the information of the Object Handler
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
 *  615     | 17/11/2008  | mcoutinho    | IPR 64
 *  3539    | 29/06/2009  | mcoutinho    | IPR 531
 *  4774    | 09/10/2009  | mcoutinho    | IPR 808
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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
#include <string.h>  /* for memcpy() */


void _Objects_Extend_information(
                                 Objects_Information *information
                                 )
{
    /* iterator object to initialize */
    Objects_Control *the_object;

    /* address for the object name */
    void *name_area;

    /* chain of inactive objects */
    Chain_Control Inactive;

    /* block count
     * @todo: this variable was related to unlimited objects. Can be removed */
    uint32_t block_count;

    /* block
     * @todo: this variable was related to unlimited objects. Can be removed */
    uint32_t block;

    /* smallest index 
     * @todo: this variable was related to unlimited objects. It is now always 
     * equal to minimum index. Can be removed */
    uint32_t index_base;

    /* minimum index */
    uint32_t minimum_index;

    /* iterator of the object to initialize index */
    uint32_t index;


    /* get the minimum index */
    minimum_index = _Objects_Get_index(information->minimum_id);

    /* the base index is the minimum index
     * @todo: can be removed */
    index_base = minimum_index;

    /* reset the block
     * @todo: can be removed */
    block = 0;

    /* reset the block count
     * @todo: can be removed */
    block_count = 0;

    /* if the index_base is the maximum we need to grow the tables */
    if(index_base >= information->maximum)
    {
        /* interrupt level */
        ISR_Level level;

        /* address of the objects */
        void **object_blocks;

        /* address for object information name table */
        Objects_Name *name_table;

        /* address of the inactive per block
         * @todo: this variable was related with unlimited objects. can be removed */
        uint32_t *inactive_per_block;

        /* table for the objects */
        Objects_Control **local_table;

        /* maximum index */
        uint32_t maximum;

        /* growing the tables means allocating a new area, doing a copy and
         * updating the information table.
         *  The allocation has :
         *      void            *objects[block_count];
         *      uint32_t         inactive_count[block_count];
         *      Objects_Name    *name_table[block_count];
         *      Objects_Control *local_table[maximum]; */

        /* up the block count */
        block_count++;

        /* update the maximum */
        maximum = information->maximum + information->allocation_size;

        /* allocate the tables and break it up */
        object_blocks = (void**) _Workspace_Allocate_or_fatal_error(block_count *
                                                                    ( sizeof (void *) + sizeof (uint32_t ) + sizeof (Objects_Name * ) ) +
                                                                    ( ( maximum + minimum_index ) * sizeof (Objects_Control * ) ));


        /* break the block into the various sections */

        /* determine the inactive per block address */
        inactive_per_block = (uint32_t *) _Addresses_Add_offset(object_blocks ,
                                                                block_count * sizeof (void*));

        /* determine the address of the name table */
        name_table = (Objects_Name *) _Addresses_Add_offset(inactive_per_block ,
                                                            block_count * sizeof (uint32_t ));

        /* determine the address of the local table */
        local_table = (Objects_Control **) _Addresses_Add_offset(name_table ,
                                                                 block_count * sizeof (Objects_Name * ));

        /* take the block count down. Saves all the (block_count - 1) in the copies */
        block_count--;

        /* deal with the special case of the 0 to minimum_index */
        for(index = 0; index < minimum_index; index++)
        {
            local_table[ index ] = NULL;
        }

        /* initialise the new entries in the table */

        object_blocks[block_count] = NULL;
        inactive_per_block[block_count] = 0;
        name_table[block_count] = NULL;

        /* initialize the local table */

        /* loop through every object */
        for(index = index_base;
            index < ( information->allocation_size + index_base );
            index++)
        {
            /* and reset the local table for the object */
            local_table[ index ] = NULL;
        }

        /* enter critical section */
        _ISR_Disable(level);

        /* initialize the table of inactive object blocks */
        information->object_blocks = object_blocks;

        /* initialize the number of inactive objects per block */
        information->inactive_per_block = inactive_per_block;

        /* initialize the local table of objects names */
        information->name_table = name_table;

        /* initialize the local table of objects */
        information->local_table = local_table;

        /* initialize the maximum number of objects of the class */
        information->maximum = maximum;

        /* initialize the maximum identifier number for objects of this class */
        information->maximum_id = _Objects_Build_id(information->the_api ,
                                                    information->the_class ,
#if defined(RTEMS_MULTIPROCESSING)
            _Objects_Local_node ,
#endif
            information->maximum);

        /* leave critical section */
        _ISR_Enable(level);

        /* up the block count */
        block_count++;
    }

    /* allocate the name table, and the objects */
    information->object_blocks[ block ] =
        _Workspace_Allocate_or_fatal_error(( information->allocation_size * information->name_length ) +
                                           ( information->allocation_size * information->size ));

    /* determine the name area address */
    name_area = (Objects_Name *) _Addresses_Add_offset(information->object_blocks[ block ] ,
                                                       ( information->allocation_size * information->size ));

    information->name_table[ block ] = name_area;

    /* initialize objects .. add to a local chain first */
    _Chain_Initialize(&Inactive ,
                      information->object_blocks[ block ] ,
                      information->allocation_size ,
                      information->size);

    /* move from the local chain, initialise, then append to the inactive chain */

    index = index_base;

    /* loop through the objects in the table */
    while(( the_object = (Objects_Control *) _Chain_Get(&Inactive) ) != NULL)
    {
        /* create object id */
        the_object->id = _Objects_Build_id(information->the_api ,
                                           information->the_class ,
#if defined(RTEMS_MULTIPROCESSING)
            _Objects_Local_node ,
#endif
            index);

        /* set object name area */
        the_object->name = (void *) name_area;

        /* update the name area */
        name_area = _Addresses_Add_offset(name_area , information->name_length);

        /* append the object to the chain of inactive objects */
        _Chain_Append(&information->Inactive , &the_object->Node);

        /* increase the object index (used to create the id) */
        index++;
    }

    /* set the allocation size */
    information->inactive_per_block[ block ] = information->allocation_size;

    /* update the number of inactive nodes */
    information->inactive += information->allocation_size;
}

/**  
 *  @}
 */

/**
 *  @}
 */

/**
 *  @file
 *  objectnametoid.c
 *
 *  @brief gets an object identifier from its name
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
 *  620     | 17/11/2008  | mcoutinho    | IPR 64
 *  5060    | 27/10/2009  | mcoutinho    | IPR 828
 *  5233    | 30/10/2009  | mcoutinho    | IPR 842
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  9293    | 03/01/2011  | mcoutinho    | IPR 451
 *  9293    | 03/01/2011  | mcoutinho    | IPR 2815
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
#include <rtems/rtems/support.h>


Objects_Name_or_id_lookup_errors _Objects_Name_to_id(
                                                     Objects_Information *information ,
                                                     Objects_Name name ,
#if defined(RTEMS_MULTIPROCESSING)
        uint32_t node ,
#endif
        Objects_Id *id
                                                     )
{
#if defined(RTEMS_MULTIPROCESSING)

    /* flag to search the local node */
    boolean search_local_node;
#endif

    /* object found */
    Objects_Control *the_object;

    /* iterator through the objects index */
    uint32_t index;

    /* name lenght */
    uint32_t name_length;

    /* comparator between object names */
    Objects_Name_comparators compare_them;


    /* check the identifier address */
    if(!id)
    {
        /* return invalid address */
        return OBJECTS_INVALID_ADDRESS;
    }

    /* check if it is a valid name */
    if(!rtems_is_name_valid((rtems_name) name))
    {
        /* return invalid name */
        return OBJECTS_INVALID_NAME;
    }

#if defined(RTEMS_MULTIPROCESSING)

    /* dont search the local node by default */
    search_local_node = FALSE;

    /* if should search through all the nodes or
     * the local node */
    if(information->maximum != 0 &&
       (node == OBJECTS_SEARCH_ALL_NODES ||
        node == OBJECTS_SEARCH_LOCAL_NODE ||
        _Objects_Is_local_node(node)))
    {
        /* then search the local node */
        search_local_node = TRUE;
    }
#endif

#if defined(RTEMS_MULTIPROCESSING)

    /* if search the local node */
    if(search_local_node)
    {
#endif

        /* get the name lenght */
        name_length = information->name_length;

        /* get the object comparator */

        /* if the name is a string */
        if(information->is_string)
        {
            /* then use the string comparator */
            compare_them = _Objects_Compare_name_string;
        }
        else
        {
            /* else, use the raw comparator */
            compare_them = _Objects_Compare_name_raw;
        }

        /* iterate through every entry on the table */
        for(index = 1; index <= information->maximum; index++)
        {
            /* disable thread dispatch to protect the information variable */
            _Thread_Disable_dispatch();

            /* get the object (if any) */
            the_object = information->local_table[ index ];

            /* if the object and the name are valid */
            if(the_object != NULL)
            {
                /* compare the names of the objects */
                if((*compare_them)(name , the_object->name , name_length))
                {
                    /* have the same name, found the object */

                    /* get the object id */
                    *id = the_object->id;

                    /* re-enable thread dispatch */
                    _Thread_Enable_dispatch();

                    /* return success */
                    return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
                }
                else
                {
                    /* re-enable thread dispatch */
                    _Thread_Enable_dispatch();
                }
            }
            else
            {
                /* re-enable thread dispatch */
                _Thread_Enable_dispatch();
            }
        }

        /* could not find the object on the local node */

#if defined(RTEMS_MULTIPROCESSING)
    }
#endif

    /* could not find the object */

    /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

    /* multiprocessing is enabled */

    /* if the node is local or search the local node */
    if(_Objects_Is_local_node(node) || node == OBJECTS_SEARCH_LOCAL_NODE)
    {
        /* then return invalid name */
        return OBJECTS_INVALID_NAME;
    }

    /* else, return the result of the global search */
    return ( _Objects_MP_Global_name_search(information , name , node , id));
#else
    /* multiprocessing is not enabled */

    /* did not find the object, has invalid name */
    return OBJECTS_INVALID_NAME;
#endif
}

/**  
 *  @}
 */

/**
 *  @}
 */

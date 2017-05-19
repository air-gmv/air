/**
 *  @file
 *  objectget.c
 *
 *  @brief get an object
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
 *  3621    | 02/07/2009  | mcoutinho    | IPR 533
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
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


Objects_Control *_Objects_Get(
                              Objects_Information *information ,
                              Objects_Id id ,
                              Objects_Locations *location
                              )
{
    /* object to get */
    Objects_Control *the_object;

    /* index of the object based on the id */
    uint32_t index;


    /* determine the index of the object */
    index = id - information->minimum_id + 1;

    /* check if the index is lower than the maximum value for this class */
    if(information->maximum >= index)
    {
        /* if so, then disable thread dispatch */
        _Thread_Disable_dispatch();

        /* get the object from the local table */
        if(( the_object = information->local_table[ index ] ) != NULL)
        {
            /* if the object exists, return the object */
            *location = OBJECTS_LOCAL;

            /* DONT enable thread dispatch! and return the object */
            return the_object;
        }

        /* enable thread dispatch */
        _Thread_Enable_dispatch();

        /* return the location error */
        *location = OBJECTS_ERROR;

        /* return null */
        return NULL;
    }

    /* index is wrong, return the location error */
    *location = OBJECTS_ERROR;

    /* if mutiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

    /* multiprocessing is enabled */

    /* get remote object */
    _Objects_MP_Is_remote(information , id , location , &the_object);

    /* return the object */
    return the_object;

#else

    /* multiprocessing is not enabled */

    /* index is wrong, return NULL */
    return NULL;
    
#endif
}

/**  
 *  @}
 */

/**
 *  @}
 */

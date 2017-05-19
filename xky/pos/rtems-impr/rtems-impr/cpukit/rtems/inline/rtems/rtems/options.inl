/**
 *  @file
 *  options.inl
 *
 *  @brief static inline implementation of the inlined
 *  routines from the Options Handler.
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
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

#ifndef _RTEMS_RTEMS_OPTIONS_INL
#define _RTEMS_RTEMS_OPTIONS_INL


/**
 *  @brief determine if the options is no wait
 *
 *  @param[in] option_set the option set
 *
 *  @return returns different than 0 if the RTEMS_NO_WAIT option is enabled in
 *  option_set
 */
RTEMS_INLINE_ROUTINE boolean _Options_Is_no_wait(
                                                 rtems_option option_set
                                                 )
{
   /* returns different than 0 if the option set has the event any setting
    * (wait for any event)*/
   return (option_set & RTEMS_NO_WAIT );
}


/**
 *  @brief determine fif the options is any
 *  
 *  @param[in] option_set the option set
 *
 *  @return returns different than 0 if the RTEMS_EVENT_ANY option is enabled in
 *  option_set
 */
RTEMS_INLINE_ROUTINE boolean _Options_Is_any(
                                             rtems_option option_set
                                             )
{
   /* returns different than 0 if the option set has the event any setting
    * (wait for any event)*/
   return (option_set & RTEMS_EVENT_ANY );
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */

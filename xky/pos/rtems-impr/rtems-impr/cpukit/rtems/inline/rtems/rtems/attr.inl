/**
 *  @file
 *  attr.inl
 *
 *  @brief all of the inlined routines associated
 *  with attributes.
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
 *  567     | 17/11/2008  | mcoutinho    | IPR 64
 *  567     | 17/11/2008  | mcoutinho    | IPR 69
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

#ifndef _RTEMS_RTEMS_ATTR_INL
#define _RTEMS_RTEMS_ATTR_INL


/**
 *  @brief set the attributes
 *  
 *  This function sets the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 *
 *  @param[in] new_attributes the new attributes
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns the atributes set
 */
RTEMS_INLINE_ROUTINE rtems_attribute _Attributes_Set(
                                                     rtems_attribute new_attributes ,
                                                     rtems_attribute attribute_set
                                                     )
{
   /* return the attribute set with the new attributes */
   return attribute_set | new_attributes;
}


/**
 *  @brief clear attributes
 *  
 *  This function clears the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 *
 *  @param[in] attribute_set the attribute set
 *  @param[in] mask the mask
 *
 *  @return returns the new attribute without the mask
 */
RTEMS_INLINE_ROUTINE rtems_attribute _Attributes_Clear(
                                                       rtems_attribute attribute_set ,
                                                       rtems_attribute mask
                                                       )
{
   /* clear the attributes set on the mask */
   return attribute_set & ~mask;
}


/**
 *  @brief determine if the attributes define floating point
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns different than 0 if the floating point attribute is
 *  enabled in the attribute_set.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_floating_point(
                                                           rtems_attribute attribute_set
                                                           )
{
   /* returns different than 0 if the attribute has the floating point setting */
   return ( attribute_set & RTEMS_FLOATING_POINT );
}

#if defined(RTEMS_MULTIPROCESSING)


/**
 *  @brief determine if the attributes define if is global
 *  
 *  @return returns TRUE if the global object attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns different than 0 if the global object attribute is
 *  enabled in the attribute_set.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_global(
                                                   rtems_attribute attribute_set
                                                   )
{
   /* returns different than 0 if the attribute has the global setting */
   return ( attribute_set & RTEMS_GLOBAL );
}
#endif


/**
 *  @brief determine if the attributes define is priority
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns different than 0 if the priority attribute is
 *  enabled in the attribute_set.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_priority(
                                                     rtems_attribute attribute_set
                                                     )
{
   /* returns different than 0 if the attribute has the priority setting */
   return ( attribute_set & RTEMS_PRIORITY );
}


/**
 *  @brief determine if the attributes define is binary semaphore
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns TRUE if the binary semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_binary_semaphore(
                                                             rtems_attribute attribute_set
                                                             )
{
   /* return true if the attribute set has semaphore class equal to binary semaphore */
   return (( attribute_set & RTEMS_SEMAPHORE_CLASS ) == RTEMS_BINARY_SEMAPHORE );
}


/**
 *  @brief determine if the attributes define is simple binary semaphore
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns TRUE if the simple binary semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_simple_binary_semaphore(
                                                                    rtems_attribute attribute_set
                                                                    )
{
   /* return true if the attribute set has semaphore class equal to simple binary semaphore */
   return (( attribute_set & RTEMS_SEMAPHORE_CLASS ) == RTEMS_SIMPLE_BINARY_SEMAPHORE );
}


/**
 *  @brief determine if the attributes define is counting semaphore
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns TRUE if the counting semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_counting_semaphore(
                                                               rtems_attribute attribute_set
                                                               )
{
   /* return true if the attribute set has semaphore class equal to counting semaphore */
   return (( attribute_set & RTEMS_SEMAPHORE_CLASS ) == RTEMS_COUNTING_SEMAPHORE );
}


/**
 *  @brief determine if the attributes define is inherit priority
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns different than 0 if the priority inheritance attribute
 *  is enabled in the attribute_set.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_inherit_priority(
                                                             rtems_attribute attribute_set
                                                             )
{
    /* returns different than 0 if the attribute has the priority inheritance setting */
   return ( attribute_set & RTEMS_INHERIT_PRIORITY );
}


/**
 *  @brief determine if the attributes define is priority ceiling
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns different than 0 if the priority ceiling attribute
 *  is enabled in the attribute_set.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_priority_ceiling(
                                                             rtems_attribute attribute_set
                                                             )
{
    /* returns different than 0 if the attribute has the priority ceiling setting */
   return ( attribute_set & RTEMS_PRIORITY_CEILING );
}


/**
 *  @brief determine if the attributes define is system task
 *
 *  @param[in] attribute_set the attribute set
 *
 *  @return returns different than 0 if the system task attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_system_task(
                                                        rtems_attribute attribute_set
                                                        )
{
    /* returns different than 0 if the attribute has the system task setting */
   return ( attribute_set & RTEMS_SYSTEM_TASK );
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */

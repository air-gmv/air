/**
 *  @file  rtems/score/objectmp.h
 *
 *  @brief Data Associated with the Manipulation of Global RTEMS Objects
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of Global RTEMS Objects.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OBJECTMP_H
#define _RTEMS_SCORE_OBJECTMP_H

#ifndef _RTEMS_SCORE_OBJECTIMPL_H
# error "Never use <rtems/rtems/objectmp.h> directly; include <rtems/rtems/objectimpl.h> instead."
#endif

#include <rtems/score/chainimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreObjectMP Object Handler Multiprocessing Support
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which is used to manage
 *  objects which have been declared to be globally visible.  This handler
 *  knows objects from all of the nodes in the system.
 */
/**@{*/

/**
 *  @brief Intializes the inactive global object chain
 *  based on the maximum number of global objects configured.
 *
 *  This routine intializes the inactive global object chain
 *  based on the maximum number of global objects configured.
 */
void _Objects_MP_Handler_initialization(void);

/**
 *  @brief Intializes the global object node number
 *  used in the ID field of all objects.
 *
 *  This routine intializes the global object node number
 *  used in the ID field of all objects.
 */
void _Objects_MP_Handler_early_initialization(void);

/**
 *  @brief Place the specified global object in the
 *  specified information table.
 *
 *  This routine place the specified global object in the
 *  specified information table.
 *
 *  @param[in] information points to the object information table for this
 *             object class.
 *  @param[in] the_global_object points to the object being opened.
 *  @param[in] the_name is the name of the object being opened.
 *  @param[in] the_id is the Id of the object being opened.
 *
 *  @todo This method only works for object types with 4 byte object names.
 *        It does not support variable length object names.
 */
void _Objects_MP_Open (
  Objects_Information *information,
  Objects_MP_Control  *the_global_object,
  uint32_t             the_name,
  Objects_Id           the_id
);

/**
 *  @brief  Allocates a global object control block
 *  and places it in the specified information table.
 *
 *  This routine allocates a global object control block
 *  and places it in the specified information table.  If the
 *  allocation fails, then is_fatal_error determines the
 *  error processing actions taken.
 *
 *  @param[in] information points to the object information table for this
 *             object class.
 *  @param[in] the_name is the name of the object being opened.
 *  @param[in] the_id is the Id of the object being opened.
 *  @param[in] is_fatal_error is true if not being able to allocate the
 *             object is considered a fatal error.
 *
 *  @todo This method only works for object types with 4 byte object names.
 *        It does not support variable length object names.
 */
bool _Objects_MP_Allocate_and_open (
  Objects_Information *information,
  uint32_t             the_name,
  Objects_Id           the_id,
  bool                 is_fatal_error
);

/**
 *  @brief Removes a global object from the specified information table.
 *
 *  This routine removes a global object from the specified
 *  information table and deallocates the global object control block.
 */
void _Objects_MP_Close (
  Objects_Information *information,
  Objects_Id           the_id
);

/**
 *  @brief Look for the object with the_name in the global
 *  object tables indicated by information.
 *
 *  This routine looks for the object with the_name in the global
 *  object tables indicated by information.  It returns the ID of the
 *  object with that name if one is found.
 *
 *  @param[in] information points to the object information table for this
 *             object class.
 *  @param[in] the_name is the name of the object being searched for.
 *  @param[in] nodes_to_search indicates the set of nodes to search.
 *  @param[in] the_id will contain the Id of the object if found.
 *
 *  @retval This method returns one of the
 *          @ref Objects_Name_or_id_lookup_errors.  If successful, @a the_id
 *          will contain the Id of the object.
 */
Objects_Name_or_id_lookup_errors _Objects_MP_Global_name_search (
  Objects_Information *information,
  Objects_Name         the_name,
  uint32_t             nodes_to_search,
  Objects_Id          *the_id
);

/**
 * @brief Returns true, if the object identifier is in the global object
 * identifier cache of the specified object information, otherwise false.
 *
 * @param id The object identifier.
 * @param information The object information.
 *
 * @retval true A remote objects with this object identifier exits in the
 * global object identifier cache of the specified information.
 * @retval false Otherwise.
 */
bool _Objects_MP_Is_remote(
  Objects_Id                 id,
  const Objects_Information *information
);

/**
 *  This is the maximum number of global objects configured.
 */
extern uint32_t _Objects_MP_Maximum_global_objects;

/**
 * This function allocates a Global Object control block.
 */

Objects_MP_Control *_Objects_MP_Allocate_global_object( void );

/**
 * This routine deallocates a Global Object control block.
 */

void _Objects_MP_Free_global_object( Objects_MP_Control *the_object );

/**
 * This function returns whether the global object is NULL or not.
 */

RTEMS_INLINE_ROUTINE bool _Objects_MP_Is_null_global_object (
  Objects_MP_Control *the_object
)
{
  return( the_object == NULL );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

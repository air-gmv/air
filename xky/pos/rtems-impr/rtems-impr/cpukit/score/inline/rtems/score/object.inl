/**
 *  @file
 *  object.inl
 *
 *  @brief contains the static inline implementation of all
 *  of the inlined routines in the Object Handler.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  608     | 17/11/2008  | mcoutinho    | IPR 64
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  4755    | 09/10/2009  | mcoutinho    | IPR 792
 *  4946    | 23/10/2009  | mcoutinho    | IPR 819
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
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

#ifndef _RTEMS_SCORE_OBJECT_INL
#define _RTEMS_SCORE_OBJECT_INL

#include <rtems/system.h>


/**
 *  @brief build an object id
 *  
 *  This function builds an object's id from the processor node and index
 *  values specified.
 *  
 *  @param[in] the_api indicates the API associated with this Id.
 *  @param[in] the_class indicates the class of object.
 *  It is specific to @a the_api.
 *  @param[in] node is the node where this object resides.
 *  @param[in] index is the instance number of this object.
 *  
 *  @return returns an object Id constructed from the arguments.
 */
RTEMS_INLINE_ROUTINE Objects_Id _Objects_Build_id(
                                                  Objects_APIs the_api ,
                                                  uint32_t the_class ,
#if defined(RTEMS_MULTIPROCESSING)
    uint32_t node ,
#endif
    uint32_t index
                                                  )
{
   /* return the built id:
    *    API | class | node | index
    *    API is the first bits
    *    class is next to the API
    *    if multiprocessing, then node is next to the class
    *    index is next to node or class */
   return (( (Objects_Id) the_api ) << OBJECTS_API_START_BIT ) |
       ( ( (Objects_Id) the_class ) << OBJECTS_CLASS_START_BIT ) |
#if defined(RTEMS_MULTIPROCESSING)
       ( ( (Objects_Id) node ) << OBJECTS_NODE_START_BIT ) |
#endif
       ( ( (Objects_Id) index ) << OBJECTS_INDEX_START_BIT );
}


/**
 *  @brief get an object API
 *  
 *  This function returns the API portion of the ID.
 *  
 *  @param[in] id is the object Id to be processed.
 *  
 *  @return returns an object Id constructed from the arguments.
 */
RTEMS_INLINE_ROUTINE Objects_APIs _Objects_Get_API(
                                                   Objects_Id id
                                                   )
{
   /* return the API bits of the object id */
   return (Objects_APIs) ( ( id >> OBJECTS_API_START_BIT ) & OBJECTS_API_VALID_BITS );
}


/**
 *  @brief get an object class
 *  
 *  This function returns the class portion of the ID.
 *  
 *  @param[in] id is the object Id to be processed
 *
 *  @return returns the object class
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_class(
                                                 Objects_Id id
                                                 )
{
   /* return the class bits of the object id */
   return (uint32_t) ( ( id >> OBJECTS_CLASS_START_BIT ) & OBJECTS_CLASS_VALID_BITS );
}

#if defined(RTEMS_MULTIPROCESSING)


/**
 *  @brief get an object node
 *  
 *  This function returns the node portion of the ID.
 *  
 *  @param[in] id is the object Id to be processed
 *  
 *  @return returns the node portion of an object ID.
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_node(
                                                Objects_Id id
                                                )
{
   /* return the node bits of the object id */
   return (id >> OBJECTS_NODE_START_BIT ) & OBJECTS_NODE_VALID_BITS;
}
#endif


/**
 *  @brief get object index
 *  
 *  This function returns the index portion of the ID.
 *  
 *  @param[in] id is the Id to be processed
 *  
 *  @return returns the class portion of the specified object ID.
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_index(
                                                 Objects_Id id
                                                 )
{
   /* return the index bits of the object id */
   return (id >> OBJECTS_INDEX_START_BIT ) & OBJECTS_INDEX_VALID_BITS;
}

#if defined(RTEMS_MULTIPROCESSING)


/**
 *  @brief determine if a node is the local node
 *  
 *  This function returns TRUE if the node is of the local object, and
 *  FALSE otherwise.
 *  
 *  @param[in] node is the node number and corresponds to the node number
 *  portion of an object ID.
 *  
 *  @return returns TRUE if the specified node is the local node
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Objects_Is_local_node(
                                                    uint32_t node
                                                    )
{
   /* determine if the node is equal to the local node */
   return ( node == _Objects_Local_node );
}
#endif


#if defined(RTEMS_MULTIPROCESSING)


/**
 *  @brief determine if an object is on the local node
 *  
 *  This function returns TRUE if the id is of a local object, and
 *  FALSE otherwise.
 *  
 *  @param[in] id is an object ID
 *  
 *  @return returns TRUE if the specified object Id is local
 *  and FALSE otherwise.
 *  
 *  @note On a single processor configuration, this always returns TRUE.
 */
RTEMS_INLINE_ROUTINE boolean _Objects_Is_local_id(
                                                  Objects_Id id
                                                  )
{
   /* return the result of the search to the node */
   return _Objects_Is_local_node(_Objects_Get_node(id));
}
#endif


/**
 *  @brief determine if two objects id are equal
 *  
 *  This function returns TRUE if left and right are equal,
 *  and FALSE otherwise.
 *  
 *  @param[in] left is the Id on the left hand side of the comparison
 *  @param[in] right is the Id on the right hand side of the comparison
 *  
 *  @return returns TRUE if the specified object IDs are equal
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Objects_Are_ids_equal(
                                                    Objects_Id left ,
                                                    Objects_Id right
                                                    )
{
   /* if both objects have the same id then they are equal */
   return ( left == right );
}


/**
 *  @brief get local object from its index on the information table
 *  
 *  This function returns a pointer to the local_table object
 *  referenced by the index.
 *  
 *  @param[in] information points to an Object Information Table
 *  @param[in] index is the index of the object the caller wants to access
 *  
 *  @return returns a pointer to a local object or NULL if the
 *  index is invalid.
 */
RTEMS_INLINE_ROUTINE Objects_Control *_Objects_Get_local_object(
                                                                Objects_Information *information ,
                                                                uint16_t index
                                                                )
{
   /* if the index is larger than the information table maximum number of
    * objects */
   if(index > information->maximum)
   {
      /* then return NULL */
      return NULL;
   }

   /* else, return the information table entry */
   return information->local_table[ index ];
}


/**
 *  @brief set local object on an information table on a given index
 *  
 *  This function sets the pointer to the local_table object
 *  referenced by the index.
 *  
 *  @param[out] information points to an Object Information Table
 *  @param[in] index is the index of the object the caller wants to access
 *  @param[in] the_object is the local object pointer
 */
RTEMS_INLINE_ROUTINE void _Objects_Set_local_object(
                                                    Objects_Information *information ,
                                                    uint16_t index ,
                                                    Objects_Control *the_object
                                                    )
{
   /* check the index is below the maximum */
   if(index <= information->maximum)
   {
      /* it is, so set the object */
      information->local_table[ index ] = the_object;
   }
   else
   {
      /* if it is not, launch an internal error:
       *   internal error core
       *   internal
       *   invalid object index */
      _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                               TRUE ,
                               INTERNAL_ERROR_INVALID_OBJECT_INDEX);
   }
}


/**
 *  @brief get the information of an object id
 *  
 *  This function return the information structure given
 *  an id of an object.
 *  
 *  @param[in] id is an object ID
 *  
 *  @return returns a pointer to the Object Information Table
 *  for the class of objects which corresponds to this object ID.
 */
RTEMS_INLINE_ROUTINE Objects_Information *_Objects_Get_information(
                                                                   Objects_Id id
                                                                   )
{
   /* API of the object */
   Objects_APIs the_api;

   /* class of the object */
   uint16_t the_class;


   /* get the class of the object */
   the_class = _Objects_Get_class(id);

   /* get the API of the object */
   the_api = _Objects_Get_API(id);

   /* get the object from the object information table */
   return _Objects_Information_table[ the_api ][ the_class ];
}


/**
 *  @brief open an object
 *  
 *  This function places the_object control pointer and object name
 *  in the Local Pointer and Local Name Tables, respectively.
 *  
 *  @param[in] information points to an Object Information Table
 *  @param[in] the_object is a pointer to an object
 *  @param[in] name is the name of the object to make accessible
 *
 *  @todo remove the check if it is string
 */
RTEMS_INLINE_ROUTINE void _Objects_Open(
                                        Objects_Information *information ,
                                        Objects_Control *the_object ,
                                        Objects_Name name
                                        )
{
   /* index of the object */
   uint32_t index;

   /* get the object index */
   index = _Objects_Get_index(the_object->id);

   /* set the object on the object local information table */
   _Objects_Set_local_object(information , index , the_object);

   /* if the object information is a string */
   if(information->is_string)
   {
      /* copy the name */
      the_object->name = name;
   }
   else
   {
      /* copy the name */
      the_object->name = name;
   }
}


/**
 *  @brief close an object
 *  
 *  This function removes the_object control pointer and object name
 *  in the Local Pointer and Local Name Tables.
 *  
 *  @param[in] information points to an Object Information Table
 *  @param[in] the_object is a pointer to an object
 */
RTEMS_INLINE_ROUTINE void _Objects_Close(
                                         Objects_Information *information ,
                                         Objects_Control *the_object
                                         )
{
   /* object index */
   uint32_t index;

   /* get the object index */
   index = _Objects_Get_index(the_object->id);

   /* reset the object inforation table entry */
   _Objects_Set_local_object(information , index , NULL);

   /* reset the object name */
   the_object->name = 0;
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */

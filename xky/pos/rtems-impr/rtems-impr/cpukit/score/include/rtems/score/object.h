/**
 *  @file
 *  object.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Object Handler.  This Handler provides mechanisms which
 *  can be used to initialize and manipulate all objects which have
 *  ids.
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
 *  603     | 17/11/2008  | mcoutinho    | IPR 64
 *  3542    | 29/06/2009  | mcoutinho    | IPR 531
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5951    | 11/01/2010  | mcoutinho    | IPR 1048
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_OBJECT_H
#define _RTEMS_SCORE_OBJECT_H

/**
 *  @defgroup ScoreObject Object Handler
 *
 *   @brief This Handler provides mechanisms which  can be used to initialize
 *  and manipulate all objects which have ids.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/interr.h>

   /**
    *  @brief defines the control block used to manage
    *  object names.
    */
   typedef void * Objects_Name;

   /**
    *  @brief Space for object names is allocated in multiples of this.
    *
    *  @note Must be a power of 2. Matches the name manipulation routines.
    */
#define OBJECTS_NAME_ALIGNMENT     sizeof( uint32_t )

   /**
    *  @brief compare names prototype functions
    */
   typedef boolean(*Objects_Name_comparators)(
                                               void * /* name_1 */ ,
                                               void * /* name_2 */ ,
                                               uint16_t /* length */
                                               );

#if defined(RTEMS_USE_16_BIT_OBJECT)
   /**
    *  The following type defines the control block used to manage
    *  object IDs.  The format is as follows (0=LSB):
    *
    *     Bits  0 ..  7    = index  (up to 254 objects of a type)
    *     Bits  8 .. 10    = API    (up to 7 API classes)
    *     Bits 11 .. 15    = class  (up to 31 object types per API)
    */
   typedef uint16_t Objects_Id;

   /**
    * This type is used to store the maximum number of allowed objects
    * of each type.
    */
   typedef uint8_t Objects_Maximum;

#define OBJECTS_INDEX_START_BIT  0
#define OBJECTS_API_START_BIT    8
#define OBJECTS_CLASS_START_BIT 11

#define OBJECTS_INDEX_MASK      (Objects_Id)0x00ff
#define OBJECTS_API_MASK        (Objects_Id)0x0700
#define OBJECTS_CLASS_MASK      (Objects_Id)0xF800

#define OBJECTS_INDEX_VALID_BITS  (Objects_Id)0x00ff
#define OBJECTS_API_VALID_BITS    (Objects_Id)0x0007
   /* OBJECTS_NODE_VALID_BITS should not be used with 16 bit Ids */
#define OBJECTS_CLASS_VALID_BITS  (Objects_Id)0x001f

#define OBJECTS_ID_INITIAL_INDEX  (0)
#define OBJECTS_ID_FINAL_INDEX    (0xff)

#else
   /**
    *  @brief defines the control block used to manage
    *  object IDs.  The format is as follows (0=LSB):
    *
    *     Bits  0 .. 15    = index  (up to 65535 objects of a type)
    *     Bits 16 .. 23    = node   (up to 255 nodes)
    *     Bits 24 .. 26    = API    (up to 7 API classes)
    *     Bits 27 .. 31    = class  (up to 31 object types per API)
    */
   typedef uint32_t Objects_Id;

   /**
    * @brief store the maximum number of allowed objects of each type.
    */
   typedef uint16_t Objects_Maximum;

   /**
    * @brief bit position of the starting bit of the index portion of
    *  the object Id.
    */
#define OBJECTS_INDEX_START_BIT  0

   /**
    *  @brief bit position of the starting bit of the node portion of
    *  the object Id.
    */
#define OBJECTS_NODE_START_BIT  16

   /**
    *  @brief bit position of the starting bit of the API portion of
    *  the object Id.
    */
#define OBJECTS_API_START_BIT   24

   /**
    *  @brief bit position of the starting bit of the class portion of
    *  the object Id.
    */
#define OBJECTS_CLASS_START_BIT 27

   /**
    *  @brief mask to extract the index portion of an object Id.
    */
#define OBJECTS_INDEX_MASK      (Objects_Id)0x0000ffff

   /**
    *  @brief mask to extract the node portion of an object Id.
    */
#define OBJECTS_NODE_MASK       (Objects_Id)0x00ff0000

   /**
    *  @brief mask used to extract the API portion of an object Id.
    */
#define OBJECTS_API_MASK        (Objects_Id)0x07000000

   /**
    *  @brief mask is used to extract the class portion of an object Id.
    */
#define OBJECTS_CLASS_MASK      (Objects_Id)0xf8000000

   /**
    *  @brief mask that represents the bits that is used to ensure no extra bits
    *  are set after shifting to extract the index portion of an object Id.
    */
#define OBJECTS_INDEX_VALID_BITS  (Objects_Id)0x0000ffff

   /**
    *  @brief mask that represents the bits that is used to ensure no extra bits
    *  are set after shifting to extract the node portion of an object Id.
    */
#define OBJECTS_NODE_VALID_BITS   (Objects_Id)0x000000ff

   /**
    *  @brief mask that represents the bits that is used to ensure no extra bits
    *  are set after shifting to extract the API portion of an object Id.
    */
#define OBJECTS_API_VALID_BITS    (Objects_Id)0x00000007

   /**
    *  @brief mask that represents the bits that is used to ensure no extra bits
    *  are set after shifting to extract the class portion of an object Id.
    */
#define OBJECTS_CLASS_VALID_BITS  (Objects_Id)0x0000001f

   /**
    *  @brief lowest value for the index portion of an object Id.
    */
#define OBJECTS_ID_INITIAL_INDEX  (0)

   /**
    *  @brief highest value for the index portion of an object Id.
    */
#define OBJECTS_ID_FINAL_INDEX    (0xff)
#endif

   /**
    *  @brief enumerated type used in the class field of the object ID.
    */
   typedef enum
   {
      OBJECTS_NO_API = 0 ,
      OBJECTS_INTERNAL_API = 1 ,
      OBJECTS_CLASSIC_API = 2 ,
      OBJECTS_POSIX_API = 3 ,
      OBJECTS_ITRON_API = 4
   } Objects_APIs;

   /**
    * @brief macro used to generically specify the last API index.
    */
#define OBJECTS_APIS_LAST OBJECTS_ITRON_API

   /**
    *  @brief enumerated type used in the class field of the object ID
    *  for RTEMS internal object classes.
    */
   typedef enum
   {
      OBJECTS_INTERNAL_NO_CLASS = 0 ,
      OBJECTS_INTERNAL_THREADS = 1 ,
      OBJECTS_INTERNAL_MUTEXES = 2
   } Objects_Internal_API;

   /**
    * @brief macro used to generically specify the last API index.
    */
#define OBJECTS_INTERNAL_CLASSES_LAST OBJECTS_INTERNAL_MUTEXES

   /**
    * @brief enumerated type used in the class field of the object ID
    * for the RTEMS Classic API.
    */
   typedef enum
   {
      OBJECTS_CLASSIC_NO_CLASS = 0 ,
      OBJECTS_RTEMS_TASKS = 1 ,
      OBJECTS_RTEMS_TIMERS = 2 ,
      OBJECTS_RTEMS_SEMAPHORES = 3 ,
      OBJECTS_RTEMS_MESSAGE_QUEUES = 4 ,
      OBJECTS_RTEMS_PARTITIONS = 5 ,
      OBJECTS_RTEMS_REGIONS = 6 ,
      OBJECTS_RTEMS_PORTS = 7 ,
      OBJECTS_RTEMS_PERIODS = 8 ,
      OBJECTS_RTEMS_EXTENSIONS = 9 ,
      OBJECTS_RTEMS_BARRIERS = 10
   } Objects_Classic_API;

   /**
    * @brief macro used to generically specify the last API index.
    */
#define OBJECTS_RTEMS_CLASSES_LAST OBJECTS_RTEMS_BARRIERS

   /**
    *  @brief lists the locations which may be returned
    *  by _Objects_Get.  These codes indicate the success of locating
    *  an object with the specified ID.
    */
   typedef enum
   {
      OBJECTS_LOCAL = 0 , /* object is local */

#if defined(RTEMS_MULTIPROCESSING)
      OBJECTS_REMOTE = 1 , /* object is remote */
#endif
      
      OBJECTS_ERROR = 2 /* id was invalid */
   } Objects_Locations;

   /**
    *  @brief defines the callout used when a local task
    *  is extracted from a remote thread queue (i.e. it's proxy must
    *  extracted from the remote queue).
    */
   typedef void ( *Objects_Thread_queue_Extract_callout )(void *);

   /**
    *  @brief defines the Object Control Block used to manage
    *  each object local to this node.
    */
   typedef struct
   {
      /**
       * @brief This is the chain node portion of an object.
       */
      Chain_Node Node;
      /**
       * @brief This is the object's ID.
       */
      Objects_Id id;
      /**
       * @brief This is the object's name.
       */
      Objects_Name name;
   } Objects_Control;

   /**
    *  @brief structure for the information used to
    *  manage each class of objects.
    *
    *  @todo: there is still much information related to unlimited objects.
    *  can be removed
    */
   typedef struct
   {
      /**
       * @brief This field indicates the API of this object class.
       */
      Objects_APIs the_api;
      /**
       * @brief This is the class of this object set.
       */
      uint16_t the_class;
      /**
       * @brief This is the minimum valid id of this object class.
       */
      Objects_Id minimum_id;
      /**
       * @brief This is the maximum valid id of this object class.
       */
      Objects_Id maximum_id;
      /**
       * @brief This is the maximum number of objects in this class.
       */
      Objects_Maximum maximum;
      /**
       * @brief This is the number of objects in a block.
       */
      uint32_t allocation_size;
      /**
       * @brief This is the size in bytes of each object instance.
       */
      size_t size;
      /**
       * @brief This points to the table of local objects.
       */
      Objects_Control **local_table;
      /**
       * @brief This points to the table of local object names.
       */
      Objects_Name *name_table;
      /**
       * @brief This is the chain of inactive control blocks.
       */
      Chain_Control Inactive;
      /**
       * @brief This is the number of objects on the Inactive list.
       */
      Objects_Maximum inactive;
      /**
       * @brief This is the number of inactive objects per block.
       */
      uint32_t *inactive_per_block;
      /**
       * @brief This is a table to the chain of inactive object memory blocks.
       */
      void **object_blocks;
      /**
       * @brief This is the TRUE if names are strings.
       */
      boolean is_string;
      /**
       * @brief This is the maximum length of names.
       */
      uint16_t name_length;
      /**
       * @brief This is this object class' method called when extracting a thread.
       */
      Objects_Thread_queue_Extract_callout extract;

#if defined(RTEMS_MULTIPROCESSING)
      /**
       * @brief This is this object class' pointer to the global name table
       */
      Chain_Control *global_table;
#endif
   } Objects_Information;

#if defined(RTEMS_MULTIPROCESSING)
   /**
    *  @brief the node number of the local node.
    */
   extern uint16_t _Objects_Local_node;
#endif

#if defined(RTEMS_MULTIPROCESSING)
   /**
    *  @brief the number of nodes in the system.
    */
   extern uint16_t _Objects_Maximum_nodes;
#endif

   /**
    *  @brief list of information blocks per API for each object
    *  class.  From the ID, we can go to one of these information blocks,
    *  and obtain a pointer to the appropriate object control block.
    */
   extern Objects_Information **_Objects_Information_table[OBJECTS_APIS_LAST + 1];

   /**
    *  @brief determine the value to specify the object itself (and not search for another)
    */
#define OBJECTS_ID_OF_SELF ((Objects_Id) 0)

   /**
    *  @brief determine the value to search in all nodes (for multiprocessing)
    */
#define OBJECTS_SEARCH_ALL_NODES   0

   /**
    *  @brief determine the value to search in all nodes except for the current one
    */
#define OBJECTS_SEARCH_OTHER_NODES 0x7FFFFFFE

   /**
    *  @brief determine the value to seatch in local node only
    */
#define OBJECTS_SEARCH_LOCAL_NODE  0x7FFFFFFF

   /**
    *  @brief deterimne the value to ask who the object is
    */
#define OBJECTS_WHO_AM_I           0

   /**
    *  @brief calculate the lowest ID for the specified api, class and node.
    */
#if defined(RTEMS_MULTIPROCESSING)
#define OBJECTS_ID_INITIAL(_api, _class, _node) \
  _Objects_Build_id( (_api), (_class), (_node), OBJECTS_ID_INITIAL_INDEX )
#else
#define OBJECTS_ID_INITIAL(_api, _class) \
  _Objects_Build_id( (_api), (_class), OBJECTS_ID_INITIAL_INDEX )
#endif

   /**
    *  @brief determine the highest object ID value
    */
#define OBJECTS_ID_FINAL           ((Objects_Id)~0)

   /**
    *  @brief initialization the Object Handler
    *
    *  @param[in] node indicates the identifying number of this node.
    *  @param[in] maximum_nodes is the maximum number of nodes in this system.
    *  @param[in] maximum_global_objects is maximum number of global objects
    *  concurrently offered in the system.
    */
   void _Objects_Handler_initialization(
#if defined(RTEMS_MULTIPROCESSING)
       uint32_t node ,
                                        uint32_t maximum_nodes ,
                                        uint32_t maximum_global_objects
#endif
       );

   /**
    *  @brief extend all object information related data structures.
    *
    *  @param[in] information object information table
    */
   void _Objects_Extend_information(
                                    Objects_Information *information
                                    );

   /**
    *  @brief initialize an object class information record.
    *
    *  SUPPORTS_GLOBAL is TRUE if the object class supports global
    *  objects, and FALSE otherwise.  Maximum indicates the number
    *  of objects required in this class and size indicates the size
    *  in bytes of each control block for this object class.  The
    *  name length and string designator are also set.  In addition,
    *  the class may be a task, therefore this information is also included.
    *
    *  @param[in] information points to an object class information block.
    *  @param[in] the_api indicates the API associated with this information block.
    *  @param[in] the_class indicates the class of object being managed
    *  by this information block.  It is specific to @a the_api.
    *  @param[in] maximum is the maximum number of instances of this object
    *  class which may be concurrently active.
    *  @param[in] size is the size of the data structure for this class.
    *  @param[in] is_string is TRUE if this object uses string style names.
    *  @param[in] maximum_name_length is the maximum length of object names.
    *  @param[in] supports_global TRUE if the objects support multiprocessing
    *  @param[in] extract the extract handler routine when using multiprocessing
    */
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
       );

   /**
    *  @brief allocate an object control block from
    *  the inactive chain of free object control blocks.
    *
    *  @param[in] information points to an object class information block.
    *
    *  @return returns the object allocated or NULL if could not allocate
    */
   Objects_Control *_Objects_Allocate(
                                      Objects_Information *information
                                      );

   /**
    *  @brief allocate the object control block
    *  specified by the index from the inactive chain of
    *  free object control blocks.
    *
    *  @param[in] information points to an object class information block.
    *  @param[in] index is the index of the object to allocate.
    *  @param[in] sizeof_control is the size of the object control block.
    *
    *  @return return the allocate object
    */
   Objects_Control *_Objects_Allocate_by_index(
                                               Objects_Information *information ,
                                               uint16_t index ,
                                               uint16_t sizeof_control
                                               );

   /**
    *  @brief free a object control block to the
    *  inactive chain of free object control blocks.
    *
    *  @param[in] information points to an object class information block.
    *  @param[in] the_object points to the object to deallocate.
    */
   void _Objects_Free(
                      Objects_Information *information ,
                      Objects_Control *the_object
                      );

   /**
    *  @brief zeroes out the name.
    *
    *  @param[in] name points to the name to be zeroed out.
    *  @param[in] length is the length of the object name field.
    */
   void _Objects_Clear_name(
                            void *name ,
                            const size_t length
                            );

   /**
    *  @brief copy a string style object name from source to destination.
    *
    *  @param[in] source is the source name to copy.
    *  @param[in] destination is the destination of the copy.
    *  @param[in] length is the number of bytes to copy.
    */
   void _Objects_Copy_name_string(
                                  const void *source ,
                                  void *destination ,
                                  const size_t length
                                  );

   /**
    *  @brief copy a raw style object name from source to destination.
    *
    *  @param[in] source is the source name to copy.
    *  @param[in] destination is the destination of the copy.
    *  @param[in] length is the number of bytes to copy.
    */
   void _Objects_Copy_name_raw(
                               const void *source ,
                               void *destination ,
                               const size_t length
                               );

   /**
    *  @brief compare the name of an object with the specified string.
    *
    *  @param[in] name_1 one name
    *  @param[in] name_2 other name
    *  @param[in] length maximum length to compare
    *
    *  @return returns TRUE on a match
    */
   boolean _Objects_Compare_name_string(
                                        void *name_1 ,
                                        void *name_2 ,
                                        uint16_t length
                                        );

   /**
    *  @brief compare two raw style object names.
    *
    *  @param[in] name_1 is the left hand name to compare.
    *  @param[in] name_2 is the right hand name to compare.
    *  @param[in] length is the length of the names to compare.
    *
    *  @return returns TRUE on a match
    */
   boolean _Objects_Compare_name_raw(
                                     void *name_1 ,
                                     void *name_2 ,
                                     uint16_t length
                                     );

   /**
    *  @brief object handler return code
    **/
   typedef enum
   {
      OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL ,
      OBJECTS_INVALID_NAME ,
      OBJECTS_INVALID_ADDRESS ,
      OBJECTS_INVALID_ID ,
      OBJECTS_INVALID_NODE
   } Objects_Name_or_id_lookup_errors;

   /**
    *  @brief define the first entry in the
    *  @ref Objects_Name_or_id_lookup_errors enumerated list.
    */
#define OBJECTS_NAME_ERRORS_FIRST OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL

   /**
    *  @brief define the last entry in the
    *  @ref Objects_Name_or_id_lookup_errors enumerated list.
    */
#define OBJECTS_NAME_ERRORS_LAST  OBJECTS_INVALID_NODE

   /**
    *  @brief search the object table(s) for the given
    *  object name and returns the associated object id.
    *
    *  @param[in] information object information
    *  @param[in] name user defined object name
    *  @param[in] node node indentifier (0 indicates any node)
    *  @param[in] id address of return ID
    *  @param[out] id object id
    *
    *  @return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL if successful.
    *  error code if unsuccessful
    */
   Objects_Name_or_id_lookup_errors _Objects_Name_to_id(
                                                        Objects_Information *information ,
                                                        Objects_Name name ,
#if defined(RTEMS_MULTIPROCESSING)
       uint32_t node ,
#endif
       Objects_Id *id
                                                        );

   /**
    *  @brief implement the common portion of the object Id
    *  to name directives.  This function returns the name
    *  associated with object id.
    *
    *  @param[in] id is the Id of the object whose name we are locating.
    *  @param[in] name will contain the name of the object, if found.
    *
    *  @return This method returns one of the values from the
    *  @ref Objects_Name_or_id_lookup_errors enumeration to indicate
    *  successful or failure.  On success @a name will contain the name of
    *  the requested object.
    *
    *  @note This function currently does not support string names.
    */
   Objects_Name_or_id_lookup_errors _Objects_Id_to_name(
                                                        Objects_Id id ,
                                                        Objects_Name *name
                                                        );

   /**
    *  @brief map object ids to object control blocks.
    *
    *  If id corresponds to a local object, then it returns
    *  the_object control pointer which maps to id and location
    *  is set to OBJECTS_LOCAL.  If the object class supports global
    *  objects and the object id is global and resides on a remote
    *  node, then location is set to OBJECTS_REMOTE, and the_object
    *  is undefined.  Otherwise, location is set to OBJECTS_ERROR
    *  and the_object is undefined.
    *
    *  This function disables thread dispatch IF the object could be found.
    *  Otherwise, thread dispatch is not disabled.
    *
    *  @param[in] information points to an object class information block.
    *  @param[in] id is the Id of the object whose name we are locating.
    *  @param[in] location will contain an indication of success or failure.
    *
    *  @return This method returns one of the values from the
    *  @ref Objects_Name_or_id_lookup_errors enumeration to indicate
    *  successful or failure.  On success @a id will contain the Id of
    *  the requested object.
    *
    *  @note _Objects_Get returns with dispatching disabled for
    *  local and remote objects.  _Objects_Get_isr_disable returns with
    *  dispatching disabled for remote objects and interrupts for local
    *  objects.
    */
   Objects_Control *_Objects_Get(
                                 Objects_Information *information ,
                                 Objects_Id id ,
                                 Objects_Locations *location
                                 );

   /**
    *  @brief map object ids to object control blocks with interrupts disabled
    *
    *  If id corresponds to a local object, then it returns
    *  the_object control pointer which maps to id and location
    *  is set to OBJECTS_LOCAL.  If the object class supports global
    *  objects and the object id is global and resides on a remote
    *  node, then location is set to OBJECTS_REMOTE, and the_object
    *  is undefined.  Otherwise, location is set to OBJECTS_ERROR
    *  and the_object is undefined.
    *
    *  @param[in] information points to an object class information block.
    *  @param[in] id is the Id of the object whose name we are locating.
    *  @param[in] location will contain an indication of success or failure.
    *  @param[out] level is the interrupt level being turned.
    *
    *  @return This method returns one of the values from the
    *  @ref Objects_Name_or_id_lookup_errors enumeration to indicate
    *  successful or failure.  On success @a name will contain the name of
    *  the requested object.
    *
    *  @note _Objects_Get returns with dispatching disabled for
    *  local and remote objects.  _Objects_Get_isr_disable returns with
    *  dispatching disabled for remote objects and interrupts for local
    *  objects.
    */
   Objects_Control *_Objects_Get_isr_disable(
                                             Objects_Information *information ,
                                             Objects_Id id ,
                                             Objects_Locations *location ,
                                             ISR_Level *level
                                             );

   /**
    *  @brief map object index to object control blocks which must be local.
    *
    *  The parameter the_object control pointer which maps to id
    *  and location is set to OBJECTS_LOCAL.  Otherwise, location is set to
    *  OBJECTS_ERROR and the_object is undefined.
    *
    *  @param[in] information points to an object class information block.
    *  @param[in] id is the Id of the object whose name we are locating.
    *  @param[in] location will contain an indication of success or failure.
    *
    *  @return This method returns one of the values from the
    *  @ref Objects_Name_or_id_lookup_errors enumeration to indicate
    *  successful or failure.  On success @a id will contain the id of
    *  the requested object.
    *
    *  @note _Objects_Get returns with dispatching disabled for
    *  local and remote objects.  _Objects_Get_isr_disable returns with
    *  dispatching disabled for remote objects and interrupts for local
    *  objects.
    */
   Objects_Control *_Objects_Get_by_index(
                                          Objects_Information *information ,
                                          Objects_Id id ,
                                          Objects_Locations *location
                                          );

   /**
    *  @brief get object ids to object control blocks with no protection.
    *
    *  If id corresponds to a local object, then it returns
    *  the_object control pointer which maps to id and location
    *  is set to OBJECTS_LOCAL.  If the object class supports global
    *  objects and the object id is global and resides on a remote
    *  node, then location is set to OBJECTS_REMOTE, and the_object
    *  is undefined.  Otherwise, location is set to OBJECTS_ERROR
    *  and the_object is undefined.
    *
    *  @param[in] information points to an object class information block.
    *  @param[in] id is the Id of the object whose name we are locating.
    *  @param[in] location will contain an indication of success or failure.
    *
    *  @return This method returns one of the values from the
    *  @ref Objects_Name_or_id_lookup_errors enumeration to indicate
    *  successful or failure.  On success @a id will contain the Id of
    *  the requested object.
    *
    *  @note _Objects_Get returns with dispatching disabled for
    *  local and remote objects.  _Objects_Get_isr_disable returns with
    *  dispatching disabled for remote objects and interrupts for local
    *  objects.
    */
   Objects_Control *_Objects_Get_no_protection(
                                               Objects_Information *information ,
                                               Objects_Id id ,
                                               Objects_Locations *location
                                               );

   /**
    *  @brief get object name of an object and returns its name
    *  in the form of a C string.
    *
    *  It attempts to be careful about
    *  overflowing the user's string and about returning unprintable characters.
    *
    *  @param[in] id is the object to obtain the name of
    *  @param[in] length indicates the length of the caller's buffer
    *  @param[out] name is a string which will be filled in.
    *
    *  @return This method returns @a name or NULL on error.
    */
   char *_Objects_Get_name_as_string(
                                     Objects_Id id ,
                                     size_t length ,
                                     char *name
                                     );

   /*
    *  Pieces of object.inl are promoted out to the user
    */

#include <rtems/score/object.inl>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @file
 *  thread.inl
 *
 *  @brief contains the macro implementation of the inlined
 *  routines from the Thread handler.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  234     | 09/10/2008  | mcoutinho    | IPR 67
 *  610     | 17/11/2008  | mcoutinho    | IPR 64
 *  610     | 17/11/2008  | mcoutinho    | IPR 71
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6055    | 03/02/2010  | mcoutinho    | IPR 1305
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreThread Thread Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_THREAD_INL
#define _RTEMS_SCORE_THREAD_INL

#include <rtems/system.h>


/**
 *  @brief stop mulitasking
 *  
 *  This routine halts multitasking and returns control to
 *  the "thread" (i.e. the BSP) which initially invoked the
 *  routine which initialized the system.
 */
RTEMS_INLINE_ROUTINE void _Thread_Stop_multitasking(void)
{
   /* stop multitasksing by setting the context to the initial context */
   _Context_Switch(&_Thread_Executing->Registers , &_Thread_BSP_context);
}


/**
 *  @brief determine if a thread is the current executing thread
 *  
 *  This function returns TRUE if the_thread is the currently executing
 *  thread, and FALSE otherwise.
 *
 *  @param[in] the_thread the thread to determine if it is equal to the executing thread
 *
 *  @return returns TRUE if the thread is equal to the executing thread and
 *  FALSE otherwise
 */
RTEMS_INLINE_ROUTINE boolean _Thread_Is_executing(
                                                  const Thread_Control *the_thread
                                                  )
{
   /* thread is executing if is equal to _Thread_Executing */
   return ( the_thread == _Thread_Executing );
}


/**
 *  @brief determine if a thread is the heir thread
 *  
 *  This function returns TRUE if the_thread is the heir
 *  thread, and FALSE otherwise.
 *
 *  @param[in] the_thread the thread to determine if it is equal to the heir thread
 *
 *  @return returns TRUE if the thread is equal to the heir thread
 */
RTEMS_INLINE_ROUTINE boolean _Thread_Is_heir(
                                             const Thread_Control *the_thread
                                             )
{
   /* thread is heir if is equal to _Thread_Heir */
   return ( the_thread == _Thread_Heir );
}


/**
 *  @brief determine if the executing thread is also the heir thread
 *  
 *  This function returns TRUE if the currently executing thread
 *  is also the heir thread, and FALSE otherwise.
 *
 *  @return returns TRUE if the executing thread is equal to the heir thread
 */
RTEMS_INLINE_ROUTINE boolean _Thread_Is_executing_also_the_heir(void)
{
   /* thread executing is heir if both variables are equal */
   return ( _Thread_Executing == _Thread_Heir );
}


/**
 *  @brief unblock a thread
 *  
 *  This routine clears any blocking state for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  @param[in] the_thread the thread to unblock
 */
RTEMS_INLINE_ROUTINE void _Thread_Unblock(
                                          Thread_Control *the_thread
                                          )
{
   /* remove the blocked states from the thread */
   _Thread_Clear_state(the_thread , STATES_BLOCKED);
}


/**
 *  @brief restart the executing thread
 *  
 *  This routine resets the current context of the calling thread
 *  to that of its initial state.
 */
RTEMS_INLINE_ROUTINE void _Thread_Restart_self(void)
{

   /* if the CPU has software of hardware FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

   /* then check if thread is FP */
   if(_Thread_Executing->fp_context != NULL)
   {
      /* if it is, restore the FP context */
      _Context_Restore_fp(&_Thread_Executing->fp_context);
   }

   /* if the thread is not FP, then nothing needs to be done */

#endif

   /* restart the thread context */
   _CPU_Context_Restart_self(&_Thread_Executing->Registers);
}


/**
 *  @brief calculate the heir thread
 *  
 *  This function returns a pointer to the highest priority
 *  ready thread.
 */
RTEMS_INLINE_ROUTINE void _Thread_Calculate_heir(void)
{
   /* caclulate the heir thread based on the highest priority ready thread */
   _Thread_Heir = (Thread_Control *) _Thread_Ready_chain[ _Priority_Get_highest() ].first;
}


/**
 *  @brief disable thread dispatching
 *  
 *  This routine prevents dispatching.
 */
RTEMS_INLINE_ROUTINE void _Thread_Disable_dispatch(void)
{
   /* disable dispatch */
   _Thread_Dispatch_disable_level += 1;

   /* enforce the memory barrier to avoid optimizations */
   RTEMS_COMPILER_MEMORY_BARRIER();
}


#if ( CPU_INLINE_ENABLE_DISPATCH == TRUE )

/**
 *  @brief enable thread dispatching
 *
 *  This routine allows dispatching to occur again.  If this is
 *  the outer most dispatching critical section, then a dispatching
 *  operation will be performed and, if necessary, control of the
 *  processor will be transferred to the heir thread.
 */
RTEMS_INLINE_ROUTINE void _Thread_Enable_dispatch(void)
{
   /* enforce the memory barrier to avoid optimizations */
   RTEMS_COMPILER_MEMORY_BARRIER();

   /* check if the thread dispatch disabled level is now zero */
   if(( --_Thread_Dispatch_disable_level ) == 0)
   {
      /* if it is, then dispatch the threads */
      _Thread_Dispatch();
   }
}
#endif

#if ( CPU_INLINE_ENABLE_DISPATCH == FALSE )
void _Thread_Enable_dispatch(void);
#endif


/**
 *  @brief unnest thread dispatching
 *  
 *  This routine allows dispatching to occur again.  However,
 *  no dispatching operation is performed even if this is the outer
 *  most dispatching critical section.
 */
RTEMS_INLINE_ROUTINE void _Thread_Unnest_dispatch(void)
{
   /* enforce the memory barrier to avoid optimizations */
   RTEMS_COMPILER_MEMORY_BARRIER();

   /* decrement the thread dispatch disable level */
   _Thread_Dispatch_disable_level -= 1;
}


/**
 *  @brief determine if thread dispatching is enabled
 *
 *  @return returns TRUE if dispatching is disabled, and FALSE
 *  otherwise
 */
RTEMS_INLINE_ROUTINE boolean _Thread_Is_dispatching_enabled(void)
{
   /* check if the thread dispatch disable level is 0 */
   return ( _Thread_Dispatch_disable_level == 0 );
}


/**
 *  @brief determine if context switch is necessary
 *  
 *  @return returns TRUE if dispatching is disabled, and FALSE
 *  otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Thread_Is_context_switch_necessary(void)
{
   /* return the variable _Context_Switch_necessary */
   return ( _Context_Switch_necessary );
}


/**
 *  @brief initialize the thread dispatch
 *  
 *  This routine initializes the thread dispatching subsystem.
 */
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_initialization(void)
{
   /* initialize the thread dispatch disable level to 1 (disabled) */
   _Thread_Dispatch_disable_level = 1;
}


/**
 *  @brief get a thread based on its id
 *  
 *  This function maps thread IDs to thread control
 *  blocks.  If ID corresponds to a local thread, then it
 *  returns the_thread control pointer which maps to ID
 *  and location is set to OBJECTS_LOCAL.  If the thread ID is
 *  global and resides on a remote node, then location is set
 *  to OBJECTS_REMOTE, and the_thread is undefined.
 *  Otherwise, location is set to OBJECTS_ERROR and
 *  the_thread is undefined.
 *
 *  @param[in] id the thread id to get
 *  @param[out] location the location of the thread (if one exists)
 *
 *  @return returns the thread or NULL if none exists
 *
 *  @note The performance of many RTEMS services depends upon
 *  the quick execution of the "good object" path in this
 *  routine.  If there is a possibility of saving a few
 *  cycles off the execution time, this routine is worth
 *  further optimization attention.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Get(
                                                 Objects_Id id ,
                                                 Objects_Locations *location
                                                 )
{
   /* API of the thread */
   uint32_t the_api;

   /* class of the thread */
   uint32_t the_class;

   /* object information table of the thread */
   Objects_Information *information;

   /* thread (default return to NULL) */
   Thread_Control *tp = (Thread_Control *) 0;

   
   /* check first the case of getting the calling thread */

   /* check if the id is set to check the calling thread */
   if(_Objects_Are_ids_equal(id , OBJECTS_ID_OF_SELF))
   {
      /* disable thread dispatch */
      _Thread_Disable_dispatch();

      /* set the location */
      *location = OBJECTS_LOCAL;

      /* set the return value */
      tp = _Thread_Executing;
   }
   else
   {
      /* get the api */
      the_api = _Objects_Get_API(id);

      /* check if the api is valid */
      if(the_api && the_api > OBJECTS_APIS_LAST)
      {
         /* is not valid, return error */
         *location = OBJECTS_ERROR;
      }
      else
      {
         /* get the class */
         the_class = _Objects_Get_class(id);

         /* check if the class is valid */
         /* (threads are always first class) */
         if(the_class != 1)
         {
            /* class is not valid, return error */
            *location = OBJECTS_ERROR;
         }
         else
         {
            /* the api and class have been obtained and are correct */

            /* get the object information */
            information = _Objects_Information_table[ the_api ][ the_class ];

            /* if there is no information */
            if(!information)
            {
               /* then an error occurred */
               *location = OBJECTS_ERROR;
            }
            else
            {
               /* get the object information of the thread */
               tp = (Thread_Control *) _Objects_Get(information , id , location);
            }
         }
      }
   }

   /* return the thread */
   return tp;
}


#if defined(RTEMS_MULTIPROCESSING)


/**
 *  @brief determine if the thread proxy is blocking
 *
 *  @param[in] code the thread status code
 *
 *  @return status which indicates that a proxy is blocking, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Thread_Is_proxy_blocking(
                                                       uint32_t code
                                                       )
{
   /* thread is proxy blocking if code is equal to THREAD_STATUS_PROXY_BLOCKING */
   return (code == THREAD_STATUS_PROXY_BLOCKING );
}
#endif


/**
 *  @brief allocate an internal thread
 *  
 *  This routine allocates an internal thread.
 *
 *  @return returns the allocated thread
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Internal_allocate(void)
{
   /* allocate the thread object information */
   return (Thread_Control *) _Objects_Allocate(&_Thread_Internal_information);
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */

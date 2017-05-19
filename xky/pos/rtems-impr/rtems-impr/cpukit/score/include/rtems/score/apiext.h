/**
 *  @file
 *  apiext.h
 *
 *  @brief API Extensions Handler.
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
 *  3247    | 20/05/2009  | mcoutinho    | IPR 516
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_APIEXT_H
#define _RTEMS_SCORE_APIEXT_H

/**
 *  @defgroup ScoreAPIExtension API Extension Handler
 *
 *   @brief The API Extension Handler encapsulates functionality which provides mechanisms for the
 *   SuperCore to perform API specific actions without there being
 *   "up-references" from the SuperCore to APIs.
 *
 *  If these references
 *  were allowed in the implementation, the cohesion would be too high
 *  and adding an API would be more difficult.  The SuperCore is supposed
 *  to be largely independent of any API.
 */
/**@{*/

#include <rtems/score/chain.h>
#include <rtems/score/thread.h>

/**
 *  @brief definition of the the prototype of the Predriver Hook.
 */
typedef void (*API_extensions_Predriver_hook )(void);

/**
 *  @brief definition of the prototype of the Postdriver Hook.
 */
typedef void (*API_extensions_Postdriver_hook )(void);

/**
 *  @brief definition of the prototype of the Postswitch Hook.
 */
typedef void (*API_extensions_Postswitch_hook )(
                                                 Thread_Control *
                                                 );

/**
 *  @brief definition of the points at which an API
 *  can add an extension to the system initialization thread.
 */
typedef struct
{
   /**
    *  @brief This field allows this structure to be used with the Chain Handler.
    */
   Chain_Node Node;
   /**
    * @brief This field is the callout invoked during RTEMS initialization after
    * RTEMS data structures are initialized before device driver initialization
    * has occurred.
    *
    * @note If this field is NULL, no extension is invoked.
    */
   API_extensions_Predriver_hook predriver_hook;
   /**
    * @brief callout function invoked during RTEMS initialization after
    * RTEMS data structures and device driver initialization has occurred
    * but before multitasking is initiated.
    *
    * @note If this field is NULL, no extension is invoked.
    */
   API_extensions_Postdriver_hook postdriver_hook;
   /**
    * @brief This field is the callout invoked during each context switch
    * in the context of the heir thread.
    *
    * @note If this field is NULL, no extension is invoked.
    */
   API_extensions_Postswitch_hook postswitch_hook;
} API_extensions_Control;

/**
 *  @brief list of API extensions to the system initialization.
 */
extern Chain_Control _API_extensions_List;

/**
 *  @brief initialize the API Extensions Handler
 *  
 *  This routine initializes the API extension handler.
 */
void _API_extensions_Initialization(void);

/**
 *  @brief add extension set to the active set
 *  
 *  This routine adds an extension to the active set of API extensions.
 *  
 *  @param[in] the_extension is the extension set to add.
 */
void _API_extensions_Add(
                         API_extensions_Control *the_extension
                         );

/**
 *  @brief execute all Pre-Driver Extensions
 *  
 *  This routine executes all of the predriver callouts.
 */
void _API_extensions_Run_predriver(void);

/**
 *  @brief execute all Post-Driver Extensions
 *  
 *  This routine executes all of the postdriver callouts.
 */
void _API_extensions_Run_postdriver(void);


/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */

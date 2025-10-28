/*
 * =====================================================================
 *  FILE:  $URL$
 * =====================================================================
 *  PROJECT:             :  AVIOAR
 *  VERSION              :  $Revision$
 *  LANGUAGE             :  C
 *  AUTHOR               :  $LastChangedBy$
 *  COPYRIGHT            :  AVOS - GMV,S.A.
 *  COMPILER             :  GNU GCC
 *  CREATED              :  $CreationDate$
 *  CLASS                :  gpio
 *  LAST MODIFIED        :  $LastChangedDate$
 *  GENERATED FROM MODEL :  13-feb.-2018 7:53:54
 *  ORIGINAL MODEL AUTHOR:  cdss
 *
 *..................................................................
 *  DESCRIPTION
 *..................................................................
 *  HISTORY
 *      DATE            VERSION         AUTHOR              REASONS
 *
 *
 *======================================================================
 */



#if !defined(GPIO_H_INCLUDED)
#define GPIO_H_INCLUDED

#include "basicTypes.h"

/*
 * Identifier for the GPIOS in the boards
 */
typedef enum gpioId
{
   GPIO_0 = 0,
   GPIO_1,
   GPIO_2,
   GPIO_3,
   GPIO_4,
   GPIO_5,
   GPIO_6,
   GPIO_7,
   GPIO_8,
   GPIO_9,
   GPIO_10,
   GPIO_11,
   N_GPIOS
}gpioId_t;

/*
 * Status for the GPIOS.
 */
typedef enum gpioStatus
{
   GPIO_OFF = 0,
   GPIO_ON
}gpioStatus_t;

/*
 * function: gpioInit
 *
 * @brief
 * Initializes GPIO module
 *
 * WARNING!! For Seven Solution Boards, GPIOs must be initialized before CAN controller!!
 *
 * @return bool_t
 *
 * requirements:
 * No traceability yet
 */
bool_t gpioInit(void);
/*
 * function: setGpio
 *
 * @brief
 * Set the status (ON/OFF) for a GPIO
 *
 * @param[in] id (gpioId_t)
 * @param[in] status (gpioStatus_t)
 * @return void
 *
 * requirements:
 * No traceability yet
 */
void setGpio(gpioId_t id, gpioStatus_t status);
/*
 * function: getGpio
 *
 * @brief
 * Gets the current status for a GPIO
 *
 * @param[in] id (gpioId_t)
 * @return gpioStatus_t
 *
 * requirements:
 * No traceability yet
 */
gpioStatus_t getGpio(gpioId_t id);
/*
 * function: enableCanTrasceiver
 *
 * @brief
 * This functions sets the STANDBY pin of the CAN trasceiver
 * to low level in order to enable the Normal Operation of
 * the trasceiver
 *
 * @return void
 *
 * requirements:
 * No traceability yet
 */
void enableCanTrasceiver(void);



/*
 * function: applyGPIOMask
 *
 * @brief
 * This functions sets the read/write mask of pins
 *
 * @return void
 *
 * requirements:
 * No traceability yet
 */
void applyGPIOMask(uint32_t mask);

#endif /*!defined(GPIO_H_INCLUDED)*/



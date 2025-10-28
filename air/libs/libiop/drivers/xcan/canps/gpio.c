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

#include "gpio.h"
#include "xparameters.h"
#include "xgpiops.h"
#include "iop.h"

#define GPIO_DEVICE_ID     XPAR_XGPIOPS_0_DEVICE_ID
#define CAN_STB_PIN        (49u)

static XGpioPs gXilGpioPsInst;

#define GPIO_REGISTER_ADDR (XPAR_ADC_CONTROLLER_SA_OBC_0_BASEADDR + 0x0C)
static uint32_t GPIO_REG_VALUE = 0x00000000;

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
bool_t gpioInit(void)
{
   bool_t ret = TRUE;

   int32_t status;
   XGpioPs* gpioInstPtr = &gXilGpioPsInst;
   XGpioPs_Config *ConfigPtr;

#if defined(BOARD_OBC) || defined(BOARD_ECU) || defined(BOARD_N1) || defined(BOARD_N2)
   dpRegParam(DP_ID_GPIO_REG_ID,&GPIO_REG_VALUE,sizeof(GPIO_REG_VALUE),DP_UINT32);
#endif

   /*
    * Initialize Gpio manager
    */
   ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
   if (ConfigPtr == NULL)
   {
      iop_debug("GPIO device does not exist in peripherals table \n",NULL);
      ret = FALSE;
   }
   else
   {
      status = XGpioPs_CfgInitialize(gpioInstPtr, ConfigPtr, ConfigPtr->BaseAddr);
      if (status != XST_SUCCESS)
      {
         iop_debug("GPIO config failed \n",NULL);
         ret = FALSE;
      }
   }

   return ret;
}

/*
 * function: setGpio
 *
 * @brief
 * Set the status (ON/OFF) for a GPIO
 * @param[in] id (gpioId_t)
 * @param[in] status (gpioStatus_t)
 * @return void
 *
 * requirements:
 * No traceability yet
 */
void setGpio(gpioId_t id, gpioStatus_t status)
{
   uint8_t gpId = (uint8_t) id;
   uint32_t mask = 0u;

   if (id < N_GPIOS)
   {
      GPIO_REG_VALUE = *(volatile uint32_t*) GPIO_REGISTER_ADDR;
      mask = 0x00000001u << gpId;
      if (status == GPIO_ON)
      {
         GPIO_REG_VALUE = GPIO_REG_VALUE | (mask);
      }
      else
      {
         GPIO_REG_VALUE = GPIO_REG_VALUE & (~mask);
      }
      /* write */
      *(volatile uint32_t*) GPIO_REGISTER_ADDR = GPIO_REG_VALUE;
      /* read */
      GPIO_REG_VALUE = *(volatile uint32_t*) GPIO_REGISTER_ADDR;
   }
}

/*
 * function: getGpio
 *
 * @brief
 * Gets the current status for a GPIO
 * @param[in] id (gpioId_t)
 * @return gpioStatus_t
 *
 * requirements:
 * No traceability yet
 */
gpioStatus_t getGpio(gpioId_t id)
{
   uint8_t gpId = (uint8_t) id;
   uint32_t mask = 0u;
   gpioStatus_t res = GPIO_OFF;

   if (id < N_GPIOS)
   {
      mask = 0x00000001u << gpId;
      GPIO_REG_VALUE = *(volatile uint32_t*) GPIO_REGISTER_ADDR;
      res = (gpioStatus_t) ((GPIO_REG_VALUE & mask) >> gpId);
   }
   return res;
}

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
void enableCanTrasceiver(void)
{
   XGpioPs* gpioInstPtr = &gXilGpioPsInst;

   XGpioPs_SetDirectionPin(gpioInstPtr, CAN_STB_PIN, 1);
   XGpioPs_SetOutputEnablePin(gpioInstPtr, CAN_STB_PIN, 1);
   XGpioPs_WritePin(gpioInstPtr, CAN_STB_PIN, 0x0);
}

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
void applyGPIOMask(uint32_t mask)
{
   XGpioPs* gpioInstPtr = &gXilGpioPsInst;
   iop_debug("  :: xGPIO :: applyGPIOMask: 0x%04x\n", mask);

   for (int i = 0; i < 32; ++i)
   {
      iop_debug("      Pin: %d  Direction: %d", i, (mask >> i)&&0x1);
      XGpioPs_SetDirectionPin(gpioInstPtr, i, (mask >> i)&&0x1);
      XGpioPs_SetOutputEnablePin(gpioInstPtr, i, (mask >> i)&&0x1);
      if ((mask >> i)&&0x1){
         XGpioPs_WritePin(gpioInstPtr, i, 0x1);
         iop_debug(" Wrote: %d", 0x1);
      }
      iop_debug("\n", 0x1);

   }
}


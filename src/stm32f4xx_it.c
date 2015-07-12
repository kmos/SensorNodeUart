/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @date    21/04/2015 20:01:31
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef TESTING
extern PCD_HandleTypeDef hpcd;
#endif

extern UART_HandleTypeDef huart5;

extern void xPortSysTickHandler(void);

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void SysTick_Handler(void)
{
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    xPortSysTickHandler();
  }
  HAL_IncTick();
}

#ifdef TESTING
void OTG_FS_IRQHandler(void)
{
   HAL_PCD_IRQHandler(&hpcd);
}
#endif

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles RCC global interrupt.
*/
void RCC_IRQHandler(void){}


void UART5_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart5);
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

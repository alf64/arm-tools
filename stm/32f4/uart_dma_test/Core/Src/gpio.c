/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
//!< flashes LEDs on start up
void GPIO_FlashLedsOnStartUp(void)
{
    const uint32_t delay_ms = 60;
    uint8_t rounds = 2;
    while(rounds)
    {
        // enable
        HAL_GPIO_TogglePin(LEDS_GPIO_Port, LD3_Pin);
        HAL_Delay(delay_ms);
        HAL_GPIO_TogglePin(LEDS_GPIO_Port, LD4_Pin);
        HAL_Delay(delay_ms);
        HAL_GPIO_TogglePin(LEDS_GPIO_Port, LD6_Pin);
        HAL_Delay(delay_ms);
        HAL_GPIO_TogglePin(LEDS_GPIO_Port, LD5_Pin);
        HAL_Delay(delay_ms);

        // disable
        HAL_GPIO_TogglePin(LEDS_GPIO_Port, LD3_Pin);
        HAL_Delay(delay_ms);
        HAL_GPIO_TogglePin(LEDS_GPIO_Port, LD4_Pin);
        HAL_Delay(delay_ms);
        HAL_GPIO_TogglePin(LEDS_GPIO_Port, LD6_Pin);
        HAL_Delay(delay_ms);
        HAL_GPIO_TogglePin(LEDS_GPIO_Port, LD5_Pin);
        HAL_Delay(delay_ms);

        rounds--;
    }
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

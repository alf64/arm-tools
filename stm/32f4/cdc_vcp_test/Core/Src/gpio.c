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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
static bool GPIO_BlueBtnBounced();
volatile bool blue_btn_bounced = FALSE;
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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = BLUE_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BLUE_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 2 */
void OnBlueButtonPress()
{
    bool btn_press_confirmed = FALSE;

    if(GPIO_BlueBtnBounced())
    {
        GPIO_PinState bluebtn = HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO_Port, BLUE_BUTTON_Pin);
        if(bluebtn == GPIO_PIN_SET)
        {
            HAL_Delay(GPIO_BLUE_BTN_DEBOUNCE_DELAY_MS);
            bluebtn = HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO_Port, BLUE_BUTTON_Pin);
            if(bluebtn != GPIO_PIN_SET)
            {
                return;
            }
            HAL_Delay(GPIO_BLUE_BTN_DEBOUNCE_DELAY_MS);
            bluebtn = HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO_Port, BLUE_BUTTON_Pin);
            if(bluebtn != GPIO_PIN_SET)
            {
                return;
            }
            else
            {
                btn_press_confirmed = TRUE;
            }
            while(HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO_Port, BLUE_BUTTON_Pin)){}; // wait for the button to be released
        }
    }

    if(btn_press_confirmed)
    {
        HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
        CDC_ToggleBlueLedMsg();
    }
}

/*
 * Returns 1 if blue button was pressed since last time user called this function.
 * Returns 0 if blue button wasn't pressed since last time user called this function.
 */
static bool GPIO_BlueBtnBounced()
{
    bool ret_val = blue_btn_bounced;
    blue_btn_bounced = FALSE;
    return ret_val;
}

/*
 * A callback for exti gpio interrupt,
 * called from HAL_GPIO_EXTI_IRQHandler,
 * which is called from EXTI0_IRQHandler.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case BLUE_BUTTON_Pin:
        {
            blue_btn_bounced = TRUE;
            break;
        }
        default:
        {
            break;
        }
    }
}

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

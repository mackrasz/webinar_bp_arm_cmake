#include "hal_led.h"

#include "stm32f4xx.h"
#include "cmsis_helper.h"

HAL_Status_T HAL_LED_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    FILL_REG(GPIOA->MODER, GPIO_MODER_MODER5, 0x1);

    return HAL_OK;
}

HAL_Status_T HAL_LED_Toggle(void)
{
    GPIOA->ODR ^= GPIO_ODR_OD5;

    return HAL_OK;
}

HAL_Status_T HAL_LED_On(void)
{
    GPIOA->ODR |= GPIO_ODR_OD5;

    return HAL_OK;
}
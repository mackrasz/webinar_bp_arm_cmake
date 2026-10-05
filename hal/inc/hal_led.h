#ifndef _HAL_LED_H
#define _HAL_LED_H

#include "hal_common.h"

HAL_Status_T HAL_LED_Init(void);
HAL_Status_T HAL_LED_Toggle(void);
HAL_Status_T HAL_LED_On(void);

#endif
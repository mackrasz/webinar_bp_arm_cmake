#include <stdint.h>

#include "hal_led.h"

int main(void)
{
    HAL_LED_Init();

    while(1)
    {
        HAL_LED_Toggle();
        
        for (volatile int i = 0; i < 100000; i++)
        {
            
        }
    }
    return 0;
}
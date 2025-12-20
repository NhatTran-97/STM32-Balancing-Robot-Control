#include "buzzer.h"

#define BUZZER_DUTY_ON   350

void Buzzer_Init(void)
{
	HAL_TIMEx_PWMN_Start(&htim8, BUZZER_CHANNEL);
	 __HAL_TIM_SET_COMPARE(&htim8, BUZZER_CHANNEL, 0);
}

/**
 * @brief  Turn on buzzer
 */
void Buzzer_On(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, BUZZER_CHANNEL, BUZZER_DUTY_ON);
}


/**
 * @brief  Turn off buzzer
 */
void Buzzer_Off(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, BUZZER_CHANNEL, 0);
}


void Buzzer_SetDuty(uint8_t percent)
{
	__HAL_TIM_SET_COMPARE(&htim8, BUZZER_CHANNEL, percent_to_duty(percent));
	
}

uint16_t percent_to_duty(uint8_t   percent)
{
     if (percent > 100) percent = 100;
		 if (percent < 0) percent = 0;
    return (uint16_t)((percent * 360) / 100);
}
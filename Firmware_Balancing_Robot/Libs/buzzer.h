#ifndef INC_BUZZER_H_ 
#define INC_BUZZER_H_
#include "main.h"

extern TIM_HandleTypeDef htim8;
#define BUZZER_CHANNEL   TIM_CHANNEL_1



// ===== API =====
void Buzzer_Init(void);    
void Buzzer_On(void);      
void Buzzer_Off(void);    
void Buzzer_SetDuty(uint8_t percent); 
uint16_t percent_to_duty(uint8_t percen);

#endif

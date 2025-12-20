#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "main.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Bi?n này PH?I du?c define ? main.c (không static)
extern volatile uint16_t adc_battery;


float Battery_GetVbat(void);

#ifdef __cplusplus
}
#endif

#endif // INC_BATTERY_H_

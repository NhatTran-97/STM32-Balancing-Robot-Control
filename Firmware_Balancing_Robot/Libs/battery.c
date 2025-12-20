#include "battery.h"

#define BATTERY_RTOP_OHM          20000.0f
#define BATTERY_RBOT_OHM          10000.0f
#define BATTERY_VREF_VOLTS        3.01f
#define BATTERY_ADC_MAX_COUNTS    4095.0f   // 12-bit

float Battery_GetVbat(void)
{
    float v_adc = ((float)adc_battery * BATTERY_VREF_VOLTS) / BATTERY_ADC_MAX_COUNTS;
    float gain  = (BATTERY_RTOP_OHM + BATTERY_RBOT_OHM) / BATTERY_RBOT_OHM; // = 3.0
    return v_adc * gain;
}

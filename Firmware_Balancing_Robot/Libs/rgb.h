#ifndef RGB_H
#define RGB_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    RGB_RED = 0,
    RGB_GREEN,
    RGB_BLUE,
    RGB_WHITE,
    RGB_YELLOW,
    RGB_PURPLE,
    RGB_OFF
} Color;

typedef struct
{
    GPIO_TypeDef *port;      
    uint16_t red_pin;        
    uint16_t green_pin;      
    uint16_t blue_pin;    

    uint8_t state;          
    uint32_t last_time_ms;   
    Color color;            
} RGB;

/**
 * @brief Init RGB v?i 1 port + 3 pin
 * @example rgb_init(&rgb, GPIOB, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2);
 */
void rgb_init(RGB *rgb, GPIO_TypeDef *port,
              uint16_t red_pin, uint16_t green_pin, uint16_t blue_pin);

/**
 * @brief Set màu (set ngay l?p t?c)
 */
void rgb_setcolor(RGB *rgb, Color color);

/**
 * @brief Blink non-blocking. G?i l?p trong while(1) ho?c tick timer.
 * @param blink_time_ms th?i gian m?i l?n d?i tr?ng thái ON/OFF (ms)
 */
void rgb_blink(RGB *rgb, Color color, uint16_t blink_time_ms);

#endif


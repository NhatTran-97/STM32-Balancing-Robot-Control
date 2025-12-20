#include "rgb.h"

/* ===================== C?U HÌNH ===================== */
/*
 * N?u RGB c?a b?n là common cathode (thu?ng g?p):
 *  - GPIO HIGH -> sáng
 * N?u common anode:
 *  - GPIO LOW -> sáng  (d?o logic)
 *
 * Ð?t RGB_ACTIVE_HIGH = 1 cho common cathode
 * Ð?t RGB_ACTIVE_HIGH = 0 cho common anode
 */
#define RGB_ACTIVE_HIGH  1

static inline void pin_on(GPIO_TypeDef *port, uint16_t pin)
{
#if RGB_ACTIVE_HIGH
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
#else
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
#endif
}

static inline void pin_off(GPIO_TypeDef *port, uint16_t pin)
{
#if RGB_ACTIVE_HIGH
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
#else
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
#endif
}

static void rgb_apply(RGB *rgb, bool r, bool g, bool b)
{
    if (!rgb || !rgb->port) return;

    if (r) pin_on(rgb->port, rgb->red_pin);   else pin_off(rgb->port, rgb->red_pin);
    if (g) pin_on(rgb->port, rgb->green_pin); else pin_off(rgb->port, rgb->green_pin);
    if (b) pin_on(rgb->port, rgb->blue_pin);  else pin_off(rgb->port, rgb->blue_pin);
}

static void rgb_toggle_internal(RGB *rgb, Color color)
{
    if (!rgb) return;

    if (rgb->state == 0)
    {
        // OFF -> ON (set color)
        rgb_setcolor(rgb, color);
        rgb->state = 1;
    }
    else
    {
        // ON -> OFF
        rgb_setcolor(rgb, RGB_OFF);
        rgb->state = 0;
    }
}

/* ===================== API ===================== */

void rgb_init(RGB *rgb, GPIO_TypeDef *port,
              uint16_t red_pin, uint16_t green_pin, uint16_t blue_pin)
{
    if (!rgb) return;

    rgb->port = port;
    rgb->red_pin = red_pin;
    rgb->green_pin = green_pin;
    rgb->blue_pin = blue_pin;

    rgb->state = 0;
    rgb->last_time_ms = HAL_GetTick();
    rgb->color = RGB_OFF;

    rgb_setcolor(rgb, RGB_OFF);
}

void rgb_setcolor(RGB *rgb, Color color)
{
    if (!rgb) return;

    rgb->color = color;

    switch (color)
    {
        case RGB_RED:    rgb_apply(rgb, true,  false, false); break;
        case RGB_GREEN:  rgb_apply(rgb, false, true,  false); break;
        case RGB_BLUE:   rgb_apply(rgb, false, false, true ); break;

        case RGB_WHITE:  rgb_apply(rgb, true,  true,  true ); break;
        case RGB_YELLOW: rgb_apply(rgb, true,  true,  false); break; // R + G
        case RGB_PURPLE: rgb_apply(rgb, true,  false, true ); break; // R + B

        case RGB_OFF:
        default:         rgb_apply(rgb, false, false, false); break;
    }
}

void rgb_blink(RGB *rgb, Color color, uint16_t blink_time_ms)
{
    if (!rgb) return;

    uint32_t now = HAL_GetTick();

    // luu màu blink
    rgb->color = color;

    if ((uint32_t)(now - rgb->last_time_ms) >= (uint32_t)blink_time_ms)
    {
        rgb->last_time_ms = now;
        rgb_toggle_internal(rgb, color);
    }
}





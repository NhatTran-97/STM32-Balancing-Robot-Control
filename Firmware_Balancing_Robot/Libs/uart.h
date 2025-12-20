#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================== CONFIG ==================
#define UART_RX_DMA_BUF_SIZE   128      // chunk DMA nh?n m?i l?n
#define UART_RX_RING_SIZE      512      // ring buffer RX (ph?i >= UART_RX_DMA_BUF_SIZE)
#define UART_TX_RING_SIZE      512      // ring buffer TX
// ===========================================

// Init module UART (g?i sau MX_USART2_UART_Init())
void UART_Init(void);

// ========== TX (send) ==========
bool UART_Send(const uint8_t* data, uint16_t len);     // g?i không block (enqueue)
bool UART_SendString(const char* s);                   // g?i chu?i

// ========== RX (receive) ==========
uint16_t UART_Available(void);                         // s? byte dang có trong RX ring
bool UART_ReadByte(uint8_t* out);                      // d?c 1 byte
uint16_t UART_Read(uint8_t* out, uint16_t max_len);    // d?c nhi?u byte

void UART_FlushRx(void);                               // xóa RX buffer

#ifdef __cplusplus
}
#endif

#endif /* INC_UART_H_ */

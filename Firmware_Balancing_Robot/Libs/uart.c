#include "uart.h"
#include <string.h>

// CubeMX t?o huart2 trong usart.c / main.c
extern UART_HandleTypeDef huart2;

// ================= RX =================
static uint8_t  s_rx_dma_buf[UART_RX_DMA_BUF_SIZE];
static volatile uint8_t  s_rx_ring[UART_RX_RING_SIZE];
static volatile uint16_t s_rx_head = 0;
static volatile uint16_t s_rx_tail = 0;

// ================= TX =================
static uint8_t  s_tx_ring[UART_TX_RING_SIZE];
static volatile uint16_t s_tx_head = 0;
static volatile uint16_t s_tx_tail = 0;
static volatile bool     s_tx_busy = false;
static volatile uint16_t s_last_tx_chunk = 0;

// ---------- helpers ----------
static inline uint16_t next_idx(uint16_t idx, uint16_t size)
{
    idx++;
    if (idx >= size) idx = 0;
    return idx;
}

static inline uint16_t count_bytes(volatile uint16_t head, volatile uint16_t tail, uint16_t size)
{
    if (head >= tail) return (uint16_t)(head - tail);
    return (uint16_t)(size - tail + head);
}

static void rx_push(uint8_t b)
{
    uint16_t next = next_idx(s_rx_head, UART_RX_RING_SIZE);

    // full -> drop oldest
    if (next == s_rx_tail)
        s_rx_tail = next_idx(s_rx_tail, UART_RX_RING_SIZE);

    s_rx_ring[s_rx_head] = b;
    s_rx_head = next;
}

static bool rx_pop(uint8_t* out)
{
    if (s_rx_tail == s_rx_head) return false;
    *out = s_rx_ring[s_rx_tail];
    s_rx_tail = next_idx(s_rx_tail, UART_RX_RING_SIZE);
    return true;
}

static void start_rx_dma_to_idle(void)
{
    // ReceiveToIdle dùng IDLE line -> c?n b?t USART2 IRQ trong NVIC (CubeMX)
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_rx_dma_buf, sizeof(s_rx_dma_buf));

    // (tùy ch?n) t?t half transfer IRQ d? d? spam
    if (huart2.hdmarx)
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
}

static void tx_kick(void)
{
    if (s_tx_busy) return;

    uint16_t available = count_bytes(s_tx_head, s_tx_tail, UART_TX_RING_SIZE);
    if (available == 0) return;

    // g?i 1 do?n liên t?c t? tail t?i cu?i m?ng ho?c t?i head
    uint16_t chunk = 0;
    if (s_tx_head > s_tx_tail)
        chunk = (uint16_t)(s_tx_head - s_tx_tail);
    else
        chunk = (uint16_t)(UART_TX_RING_SIZE - s_tx_tail);

    s_last_tx_chunk = chunk;
    s_tx_busy = true;

    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)&s_tx_ring[s_tx_tail], chunk);
}

// ================= API =================
void UART_Init(void)
{
    s_rx_head = s_rx_tail = 0;
    s_tx_head = s_tx_tail = 0;
    s_tx_busy = false;
    s_last_tx_chunk = 0;

    start_rx_dma_to_idle();
}

bool UART_Send(const uint8_t* data, uint16_t len)
{
    if (!data || len == 0) return false;

    // Critical section: head/tail dùng chung v?i ISR TX complete
    __disable_irq();

    for (uint16_t i = 0; i < len; i++)
    {
        uint16_t next = next_idx(s_tx_head, UART_TX_RING_SIZE);
        if (next == s_tx_tail)
        {
            __enable_irq();
            return false; // TX ring full
        }
        s_tx_ring[s_tx_head] = data[i];
        s_tx_head = next;
    }

    __enable_irq();

    tx_kick();
    return true;
}

bool UART_SendString(const char* s)
{
    if (!s) return false;
    return UART_Send((const uint8_t*)s, (uint16_t)strlen(s));
}

uint16_t UART_Available(void)
{
    return count_bytes(s_rx_head, s_rx_tail, UART_RX_RING_SIZE);
}

bool UART_ReadByte(uint8_t* out)
{
    if (!out) return false;
    return rx_pop(out);
}

uint16_t UART_Read(uint8_t* out, uint16_t max_len)
{
    if (!out || max_len == 0) return 0;

    uint16_t n = 0;
    while (n < max_len)
    {
        if (!rx_pop(&out[n]))
            break;
        n++;
    }
    return n;
}

void UART_FlushRx(void)
{
    s_rx_tail = s_rx_head;
}

// ================= HAL callbacks =================

// Callback cho ReceiveToIdle DMA
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart != &huart2) return;

    // Copy chunk m?i vào RX ring
    for (uint16_t i = 0; i < Size; i++)
        rx_push(s_rx_dma_buf[i]);

    // restart RX
    start_rx_dma_to_idle();
}

// Callback khi TX DMA g?i xong 1 chunk
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart != &huart2) return;

    // advance tail theo chunk v?a g?i
    __disable_irq();
    for (uint16_t i = 0; i < s_last_tx_chunk; i++)
        s_tx_tail = next_idx(s_tx_tail, UART_TX_RING_SIZE);

    s_tx_busy = false;
    __enable_irq();

    // n?u còn d? li?u trong TX ring -> kick ti?p
    tx_kick();
}

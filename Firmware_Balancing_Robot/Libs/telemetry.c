#include "telemetry.h"
#include <string.h>
#include "uart.h"

#define TL_SYNC1  0xAA
#define TL_SYNC2  0x55

#define TL_MSG_EULER  0x01
#define TL_SCALE_ANGLE  100.0f  // deg*100

volatile pid_param_t g_pid_cmd;
volatile bool g_pid_updated = false;

#pragma pack(push, 1)
typedef struct {
    uint8_t  sync1;
    uint8_t  sync2;
    uint8_t  msg_id;
    uint8_t  payload_len;
    uint16_t seq;
    uint32_t time_ms;
} TL_Header_t;

typedef struct {
    int16_t yaw;
    int16_t pitch;
    int16_t roll;
} TL_EulerPayload_t;
#pragma pack(pop)

static uint16_t s_seq = 0;

// ===== CRC16-CCITT =====
static uint16_t crc16_ccitt(const uint8_t* data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t b = 0; b < 8; b++)
        {
            if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
            else crc <<= 1;
        }
    }
    return crc;
}

static int16_t clamp_i16(int32_t v)
{
    if (v > 32767) return 32767;
    if (v < -32768) return -32768;
    return (int16_t)v;
}

// ===================== PUBLIC =====================
void Telemetry_Init(void)
{
    s_seq = 0;
    g_pid_cmd.kp = 0.0f;
    g_pid_cmd.ki = 0.0f;
    g_pid_cmd.kd = 0.0f;
    g_pid_updated = false;
}

bool Telemetry_SendEuler(const euler_angles* e)
{
    if (!e) return false;

    TL_Header_t hdr;
    TL_EulerPayload_t p;

    hdr.sync1 = TL_SYNC1;
    hdr.sync2 = TL_SYNC2;
    hdr.msg_id = TL_MSG_EULER;
    hdr.payload_len = (uint8_t)sizeof(p);
    hdr.seq = s_seq++;
    hdr.time_ms = HAL_GetTick();

    p.yaw   = clamp_i16((int32_t)(e->yaw   * TL_SCALE_ANGLE));
    p.pitch = clamp_i16((int32_t)(e->pitch * TL_SCALE_ANGLE));
    p.roll  = clamp_i16((int32_t)(e->roll  * TL_SCALE_ANGLE));

    uint8_t frame[32];
    uint16_t idx = 0;

    memcpy(&frame[idx], &hdr, sizeof(hdr));
    idx += sizeof(hdr);

    memcpy(&frame[idx], &p, sizeof(p));
    idx += sizeof(p);

    // CRC from msg_id..payload (skip sync bytes)
    uint16_t crc = crc16_ccitt(&frame[2], (uint16_t)(idx - 2));
    frame[idx++] = (uint8_t)(crc & 0xFF);
    frame[idx++] = (uint8_t)((crc >> 8) & 0xFF);

    return UART_Send(frame, idx);
}

// ================= RX =================
typedef enum {
    RX_WAIT_SYNC1,
    RX_WAIT_SYNC2,
    RX_READ_HEADER,   // msg_id + len + seq(2) + time(4) = 8 bytes
    RX_READ_PAYLOAD,
    RX_READ_CRC
} RxState_t;

static RxState_t rx_state = RX_WAIT_SYNC1;

static uint8_t  rx_hdr[8];
static uint8_t  rx_hdr_idx = 0;

static uint8_t  rx_payload[32];
static uint8_t  rx_payload_len = 0;
static uint8_t  rx_payload_idx = 0;

static uint8_t  rx_crc_bytes[2];
static uint8_t  rx_crc_idx = 0;

static void Telemetry_HandleRxFrame(uint8_t msg_id, const uint8_t* payload, uint8_t len)
{
    switch (msg_id)
    {
    case TL_CMD_SET_PID:
        if (len == sizeof(TelemetryCmdPid_t))
        {
            const TelemetryCmdPid_t* p = (const TelemetryCmdPid_t*)payload;

            g_pid_cmd.kp = p->kp / 100.0f;
            g_pid_cmd.ki = p->ki / 1000.0f;
            g_pid_cmd.kd = p->kd / 100.0f;
            g_pid_updated = true;
        }
        break;

    case TL_CMD_ENABLE:
        // robot_enable = 1;
        break;

    case TL_CMD_DISABLE:
        // robot_enable = 0;
        break;

    default:
        break;
    }
}

void Telemetry_RxByte(uint8_t b)
{
    switch (rx_state)
    {
    case RX_WAIT_SYNC1:
        if (b == TL_SYNC1) rx_state = RX_WAIT_SYNC2;
        break;

    case RX_WAIT_SYNC2:
        if (b == TL_SYNC2)
        {
            rx_hdr_idx = 0;
            rx_state = RX_READ_HEADER;
        }
        else
        {
            rx_state = RX_WAIT_SYNC1;
        }
        break;

    case RX_READ_HEADER:
        rx_hdr[rx_hdr_idx++] = b;
        if (rx_hdr_idx >= sizeof(rx_hdr))
        {
            rx_payload_len = rx_hdr[1];
            rx_payload_idx = 0;

            if (rx_payload_len > sizeof(rx_payload))
            {
                rx_state = RX_WAIT_SYNC1;
            }
            else
            {
                rx_crc_idx = 0;
                rx_state = (rx_payload_len > 0) ? RX_READ_PAYLOAD : RX_READ_CRC;
            }
        }
        break;

    case RX_READ_PAYLOAD:
        rx_payload[rx_payload_idx++] = b;
        if (rx_payload_idx >= rx_payload_len)
        {
            rx_crc_idx = 0;
            rx_state = RX_READ_CRC;
        }
        break;

    case RX_READ_CRC:
        rx_crc_bytes[rx_crc_idx++] = b;
        if (rx_crc_idx >= 2)
        {
            uint16_t crc_rx = (uint16_t)rx_crc_bytes[0] | ((uint16_t)rx_crc_bytes[1] << 8);

            uint8_t crc_buf[8 + 32];
            memcpy(crc_buf, rx_hdr, 8);
            memcpy(crc_buf + 8, rx_payload, rx_payload_len);

            uint16_t crc_calc = crc16_ccitt(crc_buf, (uint16_t)(8 + rx_payload_len));

            if (crc_calc == crc_rx)
            {
                Telemetry_HandleRxFrame(rx_hdr[0], rx_payload, rx_payload_len);
            }

            rx_state = RX_WAIT_SYNC1;
        }
        break;

    default:
        rx_state = RX_WAIT_SYNC1;
        break;
    }
}


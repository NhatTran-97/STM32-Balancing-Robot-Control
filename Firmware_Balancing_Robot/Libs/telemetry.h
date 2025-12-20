#ifndef INC_TELEMETRY_H_
#define INC_TELEMETRY_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "euler_angles.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TL_CMD_SET_PID   = 0x20,
    TL_CMD_ENABLE    = 0x21,
    TL_CMD_DISABLE   = 0x22,
} TelemetryCmdID;

typedef struct {
    int16_t kp;   // *100
    int16_t ki;   // *1000
    int16_t kd;   // *100
} TelemetryCmdPid_t;

void Telemetry_Init(void);
bool Telemetry_SendEuler(const euler_angles* e);

// RX: feed t?ng byte nh?n t? UART
void Telemetry_RxByte(uint8_t byte);

// debug/watch
typedef struct {
    float kp;
    float ki;
    float kd;
} pid_param_t;

extern volatile pid_param_t g_pid_cmd;
extern volatile bool g_pid_updated;

#ifdef __cplusplus
}
#endif

#endif /* INC_TELEMETRY_H_ */

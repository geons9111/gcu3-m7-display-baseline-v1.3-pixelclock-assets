#pragma once
/*
 * can_hal.h
 *
 * GCU3 M7 — CAN HAL
 * Service層のためのFlexCANドライバーを抽象化します。
 */

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t can_id;
    uint8_t  data[8];
    uint8_t  dlc;
    bool     is_extended;
    uint32_t timestamp_ms;
} can_hal_message_t;

typedef enum {
    CAN_HAL_OK = 0,
    CAN_HAL_NO_MESSAGE,
    CAN_HAL_ERROR_BUS,
    CAN_HAL_ERROR_OVERFLOW
} can_hal_status_t;

can_hal_status_t can_hal_init(void);
can_hal_status_t can_hal_receive(can_hal_message_t *msg);
void             can_hal_deinit(void);

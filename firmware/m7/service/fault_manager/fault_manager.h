#pragma once

typedef enum {
    FAULT_NONE = 0,
    FAULT_A55_HEARTBEAT_TIMEOUT,
    FAULT_A55_MISS_COUNT,
    FAULT_IPC_CHANNEL_FAILURE,
    FAULT_DISPLAY_LINK_FAILURE
} fault_id_t;

void fault_manager_init(void);
void fault_manager_report(fault_id_t fault);

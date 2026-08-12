#pragma once

#include <stdint.h>

#define IPC_PROTOCOL_VERSION 1U

typedef enum {
    IPC_MSG_M7_READY = 1,
    IPC_MSG_DISPLAY_READY,
    IPC_MSG_A55_READY,
    IPC_MSG_DISPLAY_HANDOVER,
    IPC_MSG_DISPLAY_ACTIVE,
    IPC_MSG_A55_HEARTBEAT,
    IPC_MSG_DISPLAY_FAULT,
    IPC_MSG_SAFE_DISPLAY
} ipc_message_type_t;

typedef struct {
    uint16_t protocol_version;
    uint16_t message_type;
    uint32_t sequence;
    uint16_t payload_length;
    uint16_t status;
} ipc_message_header_t;

void ipc_manager_init(void);
void ipc_manager_send_display_handover(void);
void ipc_manager_send_m7_ready(void);
void ipc_manager_send_display_ready(void);
void ipc_manager_send_safe_display(void);
void ipc_manager_poll(void);

/* Set by rx callback when A55_HEARTBEAT is received this poll cycle;
 * consumed by health_manager to avoid a second parallel RPMsg listener. */
uint32_t ipc_manager_last_heartbeat_sequence(void);

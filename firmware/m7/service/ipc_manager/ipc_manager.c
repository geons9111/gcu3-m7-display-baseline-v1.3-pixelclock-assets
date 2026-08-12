#include "ipc_manager.h"
#include "ipc_hal.h"

static uint32_t sequence;
static uint32_t last_heartbeat_sequence;

static ipc_message_header_t make_header(ipc_message_type_t type)
{
    ipc_message_header_t msg = {
        .protocol_version = IPC_PROTOCOL_VERSION,
        .message_type = (uint16_t)type,
        .sequence = ++sequence,
        .payload_length = 0U,
        .status = 0U
    };
    return msg;
}

static void on_rpmsg_rx(const ipc_message_header_t *header,
                         const uint8_t *payload,
                         uint16_t payload_length)
{
    (void)payload;
    (void)payload_length;

    switch ((ipc_message_type_t)header->message_type) {
    case IPC_MSG_A55_HEARTBEAT:
        last_heartbeat_sequence = header->sequence;
        break;
    default:
        /* その他のインバウンドタイプ（A55_READY, DISPLAY_ACTIVE, DISPLAY_FAULT）は、
         * 後の統合ステップでownership_managerによって直接消費されるため、
         * GATE-01自体を閉じる必要はありません。 */
        break;
    }
}

void ipc_manager_init(void)
{
    sequence = 0U;
    last_heartbeat_sequence = 0U;
    ipc_hal_init_rpmsg(on_rpmsg_rx);
}

void ipc_manager_send_m7_ready(void)
{
    ipc_message_header_t msg = make_header(IPC_MSG_M7_READY);
    ipc_hal_send(&msg);
}

void ipc_manager_send_display_ready(void)
{
    ipc_message_header_t msg = make_header(IPC_MSG_DISPLAY_READY);
    ipc_hal_send(&msg);
}

void ipc_manager_send_display_handover(void)
{
    ipc_message_header_t msg = make_header(IPC_MSG_DISPLAY_HANDOVER);
    ipc_hal_send(&msg);
}

void ipc_manager_send_safe_display(void)
{
    ipc_message_header_t msg = make_header(IPC_MSG_SAFE_DISPLAY);
    ipc_hal_send(&msg);
}

void ipc_manager_poll(void)
{
    ipc_hal_poll();
}

uint32_t ipc_manager_last_heartbeat_sequence(void)
{
    return last_heartbeat_sequence;
}

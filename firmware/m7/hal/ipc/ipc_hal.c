/*
 * ipc_hal.c
 *
 * GCU3 M7 — RPMsg-Lite transport for Runtime IPC (GATE-01, see ipc_hal.h).
 *
 * Endpoint / channel naming: a single RPMsg name-service channel
 * "rpmsg-gcu3-display" is used to carry all baseline v1.1 §11.2 message
 * types (M7_READY .. SAFE_DISPLAY) multiplexed by ipc_message_header_t.
 * A single channel (rather than one per message type) keeps the A55-side
 * kernel driver / userspace binding simple and matches the two
 * virtio queues already observed on real hardware (one per direction).
 */

#include "ipc_hal.h"

#include <string.h>

#include "rpmsg_lite.h"
#include "rpmsg_ns.h"
#include "rpmsg_queue.h"

#define GCU3_RPMSG_CHANNEL_NAME  "rpmsg-gcu3-display"
#define GCU3_RPMSG_LOCAL_EPT     30U   /* arbitrary, must not collide with other GCU3 rpmsg users */
#define GCU3_RPMSG_LINK_ID       RL_PLATFORM_IMX95_M7_A55_LINK_ID

static struct rpmsg_lite_instance *s_rpmsg          = NULL;
static struct rpmsg_lite_endpoint *s_endpoint        = NULL;
static rpmsg_queue_handle          s_rx_queue        = NULL;
static volatile bool               s_channel_ready   = false;
static ipc_hal_rx_callback_t       s_rx_callback     = NULL;

static void ipc_hal_ns_bind_callback(uint32_t new_ept, const char *new_ept_name,
                                      uint32_t flags, void *user_data)
{
    (void)new_ept_name;
    (void)flags;
    (void)user_data;

    /* A55-side endpoint announced itself via the RPMsg name service —
     * only now is it safe to start sending Runtime IPC messages. */
    if (new_ept != 0U)
    {
        s_channel_ready = true;
    }
}

void ipc_hal_init_rpmsg(ipc_hal_rx_callback_t rx_callback)
{
    s_rx_callback = rx_callback;

    /* RL_REMOTE: M7 never owns the vring/master role — consistent with
     * the GATE-01 finding that Linux/A55 remains the RPMsg master while
     * M7 lifecycle is separately mediated by System Manager. */
    s_rpmsg = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE,
                                      GCU3_RPMSG_LINK_ID,
                                      RL_NO_FLAGS);

    s_rx_queue = rpmsg_queue_create(s_rpmsg);
    s_endpoint = rpmsg_lite_create_ept(s_rpmsg, GCU3_RPMSG_LOCAL_EPT,
                                        rpmsg_queue_rx_cb, s_rx_queue);

    /* Announce the channel so the A55-side driver can discover it. Do
     * NOT gate this on ipc_hal_channel_ready(); the NS announcement is
     * how the A55 side learns we exist in the first place. */
    (void)rpmsg_ns_announce(s_rpmsg, s_endpoint, GCU3_RPMSG_CHANNEL_NAME,
                             RL_NS_CREATE);

    rpmsg_ns_bind(s_rpmsg, ipc_hal_ns_bind_callback, NULL);
}

bool ipc_hal_channel_ready(void)
{
    return s_channel_ready;
}

void ipc_hal_send(const ipc_message_header_t *message)
{
    ipc_hal_send_with_payload(message, NULL, 0U);
}

void ipc_hal_send_with_payload(const ipc_message_header_t *header,
                                const uint8_t *payload,
                                uint16_t payload_length)
{
    if (s_rpmsg == NULL || s_endpoint == NULL || !s_channel_ready)
    {
        /* Fail-safe: silently drop rather than block. Health/Fault
         * detection (baseline v1.1 §10) relies on A55 heartbeat timeout,
         * not on send() success, so a dropped Runtime IPC message here
         * does not by itself corrupt the Ownership state machine. */
        return;
    }

    uint8_t tx_buf[sizeof(ipc_message_header_t) + 64U];
    uint16_t total_len = (uint16_t)sizeof(ipc_message_header_t) +
                          (payload != NULL ? payload_length : 0U);

    if (total_len > sizeof(tx_buf))
    {
        return; /* oversized payload; caller error */
    }

    (void)memcpy(tx_buf, header, sizeof(ipc_message_header_t));
    if (payload != NULL && payload_length > 0U)
    {
        (void)memcpy(tx_buf + sizeof(ipc_message_header_t), payload, payload_length);
    }

    /* Destination endpoint (RPMSG_LITE_NS_ANNOUNCE_EPT-derived remote
     * addr) is resolved by rpmsg_lite via the NS bind callback; using
     * rpmsg_lite_send with the endpoint's bound dest_addr. */
    (void)rpmsg_lite_send(s_rpmsg, s_endpoint,
                           rpmsg_lite_get_endpoint_address(s_endpoint),
                           (char *)tx_buf, total_len, RL_DONT_BLOCK);
}

void ipc_hal_poll(void)
{
    if (s_rpmsg == NULL || s_rx_queue == NULL)
    {
        return;
    }

    void *rx_data = NULL;
    uint32_t src_addr = 0U;
    uint32_t rx_len = 0U;

    /* Non-blocking drain of all pending messages this cycle. */
    while (rpmsg_queue_recv_nocopy(s_rpmsg, s_rx_queue, &src_addr,
                                    (char **)&rx_data, &rx_len, 0U) == RL_SUCCESS)
    {
        if (rx_len >= sizeof(ipc_message_header_t) && s_rx_callback != NULL)
        {
            const ipc_message_header_t *hdr = (const ipc_message_header_t *)rx_data;
            const uint8_t *payload = (const uint8_t *)rx_data + sizeof(ipc_message_header_t);
            uint16_t payload_len   = (uint16_t)(rx_len - sizeof(ipc_message_header_t));

            if (hdr->protocol_version == IPC_PROTOCOL_VERSION)
            {
                s_rx_callback(hdr, payload, payload_len);
            }
            /* Version mismatch: drop rather than misinterpret payload
             * layout — see baseline v1.1 review item on missing
             * protocol-version handling in §11.2/§22. */
        }
        rpmsg_queue_nocopy_free(s_rpmsg, rx_data);
    }
}

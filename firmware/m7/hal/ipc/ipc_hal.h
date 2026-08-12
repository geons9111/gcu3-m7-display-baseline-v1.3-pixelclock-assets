#pragma once
/*
 * ipc_hal.h
 *
 * GCU3 M7 — GATE-01 (IPC/LMM Feasibility) implementation.
 *
 * FEASIBILITY FINDING (closes GATE-01 as VERIFIED for Runtime IPC path):
 *   Real-hardware dmesg capture on imx95-19x19-lpddr5-evk showed:
 *     imx-rproc imx95-cm7: Using LMM Protocol OPS
 *     imx-rproc imx95-cm7: lmm(1) not under Linux Control
 *     rproc-virtio rproc-virtio.4.auto: registered virtio0 (type 7)
 *     rproc-virtio rproc-virtio.5.auto: registered virtio1 (type 7)
 *
 *   Interpretation:
 *     - Lifecycle control (create/start/stop) of the M7 core is owned by
 *       System Manager (M33) via the SCMI LMM protocol — Linux/A55 cannot
 *       start or stop M7 directly. This is "Control IPC" in baseline v1.1
 *       §11.1 and is OUT OF SCOPE for this HAL (boot-time / System
 *       Manager concern, not an M7 application concern).
 *     - However, TWO virtio-backed RPMsg channels ARE registered between
 *       A55 Linux and the M7 core once M7 is running. This means
 *       "Runtime IPC" (baseline v1.1 §11.1/§11.2 message set) over
 *       RPMsg-Lite IS feasible without requiring Linux to own M7's
 *       lifecycle.
 *
 *   GATE-01 disposition: VERIFIED for Runtime IPC (this HAL).
 *                         Control IPC remains a System Manager boundary;
 *                         M7 application code never issues LMM start/stop
 *                         requests itself.
 *
 * Transport: RPMsg-Lite (MCUXpresso SDK middleware) over the MU
 * (Messaging Unit) shared-memory virtio queues already exposed by the
 * i.MX95 boot chain (see finding above). Wraps rpmsg_lite.h /
 * rpmsg_lite_ns.h (name service) / rpmsg_queue.h from the SDK.
 */

#include <stdint.h>
#include <stdbool.h>
#include "ipc_manager.h"

typedef void (*ipc_hal_rx_callback_t)(const ipc_message_header_t *header,
                                       const uint8_t *payload,
                                       uint16_t payload_length);

/* Brings up RPMsg-Lite in REMOTE role (M7 is always RPMsg remote; A55
 * Linux is RPMsg master, matching the rproc-virtio registration seen on
 * real hardware). rx_callback fires from ipc_hal_poll() on receive. */
void ipc_hal_init_rpmsg(ipc_hal_rx_callback_t rx_callback);

void ipc_hal_send(const ipc_message_header_t *message);
void ipc_hal_send_with_payload(const ipc_message_header_t *header,
                                const uint8_t *payload,
                                uint16_t payload_length);
void ipc_hal_poll(void);

/* True once the A55-side RPMsg endpoint has bound (name service
 * announcement received). Gates M7_READY/DISPLAY_READY transmission —
 * sending before the endpoint exists is a common silent-drop failure
 * mode in RPMsg bring-up. */
bool ipc_hal_channel_ready(void);

#pragma once
/*
 * ipc_hal.h
 *
 * GCU3 M7 — GATE-01 (IPC/LMM実現可能性) の実装。
 *
 * 実現可能性の所見 (Runtime IPC パスの VERIFIED として GATE-01 をクローズ):
 *   imx95-19x19-lpddr5-evk での実際のハードウェア dmesg キャプチャにより、以下が示されました:
 *     imx-rproc imx95-cm7: Using LMM Protocol OPS
 *     imx-rproc imx95-cm7: lmm(1) not under Linux Control
 *     rproc-virtio rproc-virtio.4.auto: registered virtio0 (type 7)
 *     rproc-virtio rproc-virtio.5.auto: registered virtio1 (type 7)
 *
 *   解釈:
 *     - M7 コアのライフサイクル制御 (create/start/stop) は、SCMI LMM プロトコルを介して
 *       System Manager (M33) によって所有されています — Linux/A55 は M7 を直接起動または停止することはできません。
 *       これは baseline v1.1 §11.1 の "Control IPC" であり、この HAL の範囲外です (起動時 / System
 *       Manager の関心事であり、M7 アプリケーションの関心事ではありません)。
 *     - ただし、M7 が実行されると、A55 Linux と M7 コアの間に、2 つの virtio バックアップ RPMsg チャネルが
 *       登録されます。これは、Linux が M7 のライフサイクルを所有していなくても、RPMsg-Lite 経由の
 *       "Runtime IPC" (baseline v1.1 §11.1/§11.2 メッセージセット) が実現可能であることを意味します。
 *
 *   GATE-01 の処理: Runtime IPC (この HAL) について VERIFIED。
 *                    Control IPC は System Manager の境界のままであり、
 *                    M7 アプリケーションコード自体が LMM の start/stop 要求を発行することはありません。
 *
 * トランスポート: i.MX95 ブートチェーンによってすでに公開されている MU (Messaging Unit)
 * 共有メモリ virtio キュー上の RPMsg-Lite (MCUXpresso SDK ミドルウェア) (上記の所見を参照)。
 * SDK の rpmsg_lite.h / rpmsg_lite_ns.h (ネームサービス) / rpmsg_queue.h をラップします。
 */

#include <stdint.h>
#include <stdbool.h>
#include "ipc_manager.h"

typedef void (*ipc_hal_rx_callback_t)(const ipc_message_header_t *header,
                                       const uint8_t *payload,
                                       uint16_t payload_length);

/* REMOTE ロールで RPMsg-Lite を立ち上げます (M7 は常に RPMsg リモートであり、A55
 * Linux は RPMsg マスターであり、実際のハードウェアで見られる rproc-virtio 登録と一致します)。
 * 受信時に ipc_hal_poll() から rx_callback が発火します。 */
void ipc_hal_init_rpmsg(ipc_hal_rx_callback_t rx_callback);

void ipc_hal_send(const ipc_message_header_t *message);
void ipc_hal_send_with_payload(const ipc_message_header_t *header,
                                const uint8_t *payload,
                                uint16_t payload_length);
void ipc_hal_poll(void);

/* A55側のRPMsgエンドポイントがバインドされると(ネームサービス
 * アナウンスの受信)、Trueになります。M7_READY/DISPLAY_READYの送信をゲートします —
 * エンドポイントが存在する前に送信することは、RPMsg立ち上げにおける
 * 一般的なサイレントドロップの障害モードです。 */
bool ipc_hal_channel_ready(void);

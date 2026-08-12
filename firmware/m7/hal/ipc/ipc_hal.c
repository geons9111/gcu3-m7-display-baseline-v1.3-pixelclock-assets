/*
 * ipc_hal.c
 *
 * GCU3 M7 — Runtime IPC 用の RPMsg-Lite トランスポート (GATE-01、ipc_hal.h を参照)。
 *
 * エンドポイント / チャネルの命名: ipc_message_header_t によって多重化された、
 * すべての baseline v1.1 §11.2 メッセージタイプ (M7_READY .. SAFE_DISPLAY) を
 * 転送するために、単一の RPMsg ネームサービスチャネル "rpmsg-gcu3-display" が使用されます。
 * (メッセージタイプごとに 1 つではなく) 単一のチャネルを使用することで、A55 側の
 * カーネルドライバ / ユーザスペースのバインディングをシンプルに保ち、
 * 実際のハードウェアで既に確認されている 2 つの virtio キュー (方向ごとに 1 つ) と一致させます。
 */

#include "ipc_hal.h"

#include <string.h>

#include "rpmsg_lite.h"
#include "rpmsg_ns.h"
#include "rpmsg_queue.h"

#define GCU3_RPMSG_CHANNEL_NAME  "rpmsg-gcu3-display"
#define GCU3_RPMSG_LOCAL_EPT     30U   /* 任意。他のGCU3 rpmsgユーザーと衝突してはいけません */
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

    /* A55側のエンドポイントがRPMsgネームサービスを介して自身をアナウンスしました —
     * これにより、Runtime IPCメッセージの送信を安全に開始できます。 */
    if (new_ept != 0U)
    {
        s_channel_ready = true;
    }
}

void ipc_hal_init_rpmsg(ipc_hal_rx_callback_t rx_callback)
{
    s_rx_callback = rx_callback;

    /* RL_REMOTE: M7は決してvring/masterロールを所有しません — これは、
     * Linux/A55がRPMsgマスターであり続け、M7のライフサイクルはSystem Managerによって
     * 個別に調停されるというGATE-01の所見と一致しています。 */
    s_rpmsg = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE,
                                      GCU3_RPMSG_LINK_ID,
                                      RL_NO_FLAGS);

    s_rx_queue = rpmsg_queue_create(s_rpmsg);
    s_endpoint = rpmsg_lite_create_ept(s_rpmsg, GCU3_RPMSG_LOCAL_EPT,
                                        rpmsg_queue_rx_cb, s_rx_queue);

    /* A55側のドライバが発見できるようにチャネルをアナウンスします。
     * これを ipc_hal_channel_ready() でゲートしないでください。NSアナウンスは、
     * そもそもA55側が我々の存在を知るための手段です。 */
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
        /* フェイルセーフ: ブロックするのではなく、静かに破棄します。Health/Fault
         * 検出 (baseline v1.1 §10) は send() の成功ではなく、A55ハートビートのタイムアウトに
         * 依存しているため、ここでRuntime IPCメッセージが破棄されても、
         * それ自体がOwnershipステートマシンを破壊することはありません。 */
        return;
    }

    uint8_t tx_buf[sizeof(ipc_message_header_t) + 64U];
    uint16_t total_len = (uint16_t)sizeof(ipc_message_header_t) +
                          (payload != NULL ? payload_length : 0U);

    if (total_len > sizeof(tx_buf))
    {
        return; /* サイズ超過のペイロード; 呼び出し元のエラー */
    }

    (void)memcpy(tx_buf, header, sizeof(ipc_message_header_t));
    if (payload != NULL && payload_length > 0U)
    {
        (void)memcpy(tx_buf + sizeof(ipc_message_header_t), payload, payload_length);
    }

    /* 宛先エンドポイント (RPMSG_LITE_NS_ANNOUNCE_EPT から派生したリモート
     * アドレス) は、NSバインドコールバックを介して rpmsg_lite によって解決されます。
     * エンドポイントのバインドされた dest_addr とともに rpmsg_lite_send を使用します。 */
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

    /* このサイクルでのすべての保留中メッセージのノンブロッキングドレイン。 */
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
            /* バージョン不一致: ペイロードレイアウトを誤って解釈するのではなく、破棄します
             * — §11.2/§22 におけるプロトコルバージョンの処理の欠落に関する
             * baseline v1.1 レビュー項目を参照してください。 */
        }
        rpmsg_queue_nocopy_free(s_rpmsg, rx_data);
    }
}

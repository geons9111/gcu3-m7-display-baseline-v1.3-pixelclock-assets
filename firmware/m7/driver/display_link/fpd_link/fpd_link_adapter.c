/*
 * fpd_link_adapter.c
 *
 * GCU3 M7 — DS90UB941AS-Q1 (シリアライザ、DSI-to-SerDesブリッジ、"SDSB")
 * の I2C 経由の初期化。ベースライン v1.1 §16 運用パスと、以前の
 * 高速起動レビューの REQ-M7-03/REQ-M7-05 に従います。
 *
 * 重要 — レジスタマップの免責事項:
 *   以下のレジスタアドレス/値は、一般に公開されている DS90UB941/948
 *   ファミリのレジスタレイアウト（一般設定、DSIポート設定、PLL/クロック、
 *   リンクロック状態）に従っていますが、正確なビットフィールドの値は、
 *   運用開始前に特定の DS90UB941AS-Q1 データシートリビジョンと
 *   実際のボードのストラップ/GPIO 配線に対して再確認しなければなりません。
 *   その確認なしに車両ECUにフラッシュしないでください。これはBring-up用の
 *   スケルトンであり、認定されたレジスタシーケンスではありません。
 */

#include "fpd_link_adapter.h"
#include "i2c_hal.h"

#include <stdint.h>
#include <stdbool.h>

/* M7 から見た 7 ビット I2C アドレス。DS90UB941 のデフォルト I2C アドレス。
 * ボードのアドレス選択ストラップ (ADDR_ID ピン) と照らし合わせて確認してください。 */
#define DS90UB941_I2C_ADDR   0x0CU

/* レジスタオフセット — TBD: DS90UB941AS-Q1 データシートと照らし合わせて確認すること。 */
#define REG_GENERAL_CFG      0x03U
#define REG_DSI_LANE_CFG     0x1CU
#define REG_PLL_CTRL         0x0DU
#define REG_LINK_STATUS      0x0AU
#define BIT_LINK_LOCKED      (1U << 0)

#define FPD_LINK_I2C_INSTANCE   2U       /* TBD: 回路図と照らし合わせて LPI2C インスタンスを確認すること */
#define FPD_LINK_I2C_BAUD_HZ    400000U  /* ファストモード、REQ-M7-03 レイテンシ最適化 */
#define FPD_LINK_LOCK_TIMEOUT_POLLS  50U /* REQ-M7-05: 無限ポーリングではなく、制限付きリトライ */

static bool s_initialized = false;

int fpd_link_adapter_init(void)
{
    if (i2c_hal_init(FPD_LINK_I2C_INSTANCE, FPD_LINK_I2C_BAUD_HZ) != I2C_HAL_OK)
    {
        return -1;
    }

    /* バッチ初期化シーケンス (REQ-M7-03): 1.0秒のロゴ表示バジェット
     * (ベースライン v1.1 §7) 内でリンク Bring-up のレイテンシを最小化するために、
     * レジスタごとのラウンドトリップではなく、単一トランザクションのバーストを行います。 */
    static const i2c_hal_reg_write_t init_seq[] = {
        {REG_GENERAL_CFG,  0x9CU}, /* TBD: DSI 入力を有効化、I2C パススルーをオフにする */
        {REG_DSI_LANE_CFG, 0x04U}, /* TBD: 4 レーン DSI、GCU3_DSI_LANE_COUNT と一致させる */
        {REG_PLL_CTRL,     0x01U}, /* TBD: PLL を有効化、基準クロックソースを選択する */
    };

    i2c_hal_status_t st = i2c_hal_write_sequence(DS90UB941_I2C_ADDR, init_seq,
                                                  sizeof(init_seq) / sizeof(init_seq[0]));
    if (st != I2C_HAL_OK)
    {
        return -1;
    }

    s_initialized = true;
    return 0;
}

int fpd_link_adapter_start(void)
{
    if (!s_initialized)
    {
        return -1;
    }

    /* REQ-M7-05 フェールセーフ: リンクロックの制限付きポーリングであり、
     * 無期限ブロックではありません。タイムアウト時には失敗を返し、display_link.c が
     * (REQ-M7-05 に従い) 1回リトライするか、fault_manager にエスカレーションするかを決定できるようにします。 */
    for (uint32_t i = 0; i < FPD_LINK_LOCK_TIMEOUT_POLLS; i++)
    {
        if (fpd_link_adapter_health_check() == 0)
        {
            return 0;
        }
        /* 意図的にここでのブロッキング遅延呼び出しを行いません — 呼び出し元 (boot_manager 
         * ステートマシン) がタイミング/スケジューリングを所有しています。他の起動クリティカルパスの
         * 作業を停止させないために、このドライバーはノンブロッキングのままとします。 */
    }

    return -1; /* バジェット内でリンクがロックされませんでした */
}

int fpd_link_adapter_stop(void)
{
    static const i2c_hal_reg_write_t stop_seq[] = {
        {REG_PLL_CTRL, 0x00U}, /* TBD: PLL を無効化 */
    };
    i2c_hal_status_t st = i2c_hal_write_sequence(DS90UB941_I2C_ADDR, stop_seq, 1U);
    s_initialized = false;
    return (st == I2C_HAL_OK) ? 0 : -1;
}

int fpd_link_adapter_health_check(void)
{
    uint8_t status = 0U;
    if (i2c_hal_read_reg8(DS90UB941_I2C_ADDR, REG_LINK_STATUS, &status) != I2C_HAL_OK)
    {
        return -1;
    }
    return ((status & BIT_LINK_LOCKED) != 0U) ? 0 : -1;
}

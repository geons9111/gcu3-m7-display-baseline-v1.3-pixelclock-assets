/*
 * mipi_dsi_driver.c
 *
 * GCU3 M7 — MCUXpresso SDK fsl_mipi_dsi.h 上の MIPI-DSI ホストドライバー。
 *
 * i.MX95 DSI ホストは Synopsys DesignWare DSI コントローラーです (i.MX8M/93 と同じ IP ファミリ)。SDK はこれを MIPI_DSI_Init / dsi_dpi_config_t / dsi_config_t 経由で公開します。以下のベースアドレスとクロック名は MIMX95 用の SDK の fsl_device_registers.h に従っています — 最初の実際のハードウェア Bring-up の前に、実際の mcux-sdk チェックアウトと照らし合わせて確認してください (GCU3 TODO: LPUART_BASE スタイルのプレースホルダーの注意事項は、ここでの MIPI_DSI ベース/クロック名にも等しく適用されます を参照)。
 */

#include "mipi_dsi_driver.h"

#include "fsl_mipi_dsi.h"
#include "fsl_clock.h"

static MIPI_DSI_HOST_Type *const s_dsi_base = MIPI_DSI0; /* TBD: インスタンスとボードルーティングの確認 */

static dsi_config_t          s_dsi_config;
static dsi_dphy_config_t     s_dphy_config;
static mipi_dsi_driver_config_t s_cfg;

int mipi_dsi_driver_init(const mipi_dsi_driver_config_t *config)
{
    if (config == NULL || config->lane_count == 0U || config->lane_count > 4U)
    {
        return -1;
    }
    s_cfg = *config;

    DSI_GetDefaultConfig(&s_dsi_config);
    s_dsi_config.numLanes   = s_cfg.lane_count;
    s_dsi_config.enableHsClk = true; /* 連続 HS クロック: フレームごとの DPHY ロックレイテンシを削減し、
                                         わずかな電力コストよりも高速起動を優先します -
                                         REQ-M7-02 の minimal-init に従い許容されます。 */

    /* DPHY PLL: 要求されたレーンごとの HS ビットレートから導出します。SDK の
     * ヘルパーが PLL 分周器の設定を計算します。refClkFreq は imx95_bsp_init() 
     * によって設定された DSI リファレンスクロックです。 */
    uint32_t dsi_ref_clk_hz = CLOCK_GetIpFreq(kCLOCK_Root_MipiDsiRef);
    DSI_GetDphyDefaultConfig(&s_dphy_config, s_cfg.hs_bitrate_bps, dsi_ref_clk_hz);

    status_t st = DSI_Init(s_dsi_base, &s_dsi_config);
    if (st != kStatus_Success)
    {
        return -1;
    }

    st = DSI_InitDphy(s_dsi_base, &s_dphy_config, dsi_ref_clk_hz);
    if (st != kStatus_Success)
    {
        return -1;
    }

    return 0;
}

int mipi_dsi_driver_start(void)
{
    /* ピクセルデータ送信のために HS モードに入ります。コマンドモード (LP) は
     * mipi_dsi_driver_dcs_write() での Bring-up の DCS ハンドシェイクにのみ
     * 使用されます。実行時のトラフィックの大半は、DPU ドライバーによって設定された
     * DPI->DSI データパス上のビデオモードです。 */
    DSI_SetDpiConfig(s_dsi_base, &s_dsi_config, s_cfg.lane_count,
                      s_cfg.width_px, s_cfg.height_px);
    return 0;
}

void mipi_dsi_driver_stop(void)
{
    DSI_Deinit(s_dsi_base);
}

int mipi_dsi_driver_dcs_write(uint8_t dcs_cmd, const uint8_t *data, uint16_t length)
{
    dsi_transfer_t xfer = {0};
    xfer.txDataSize = length;
    xfer.txData     = data;
    xfer.sendCmd    = dcs_cmd;
    xfer.subLpm     = false;

    status_t st = (length <= 2U)
                      ? DSI_TransferWrite(s_dsi_base, &xfer)   /* ショート DCS 書き込み */
                      : DSI_TransferWrite(s_dsi_base, &xfer);  /* ロング DCS 書き込み。SDK が
                                                                   txDataSize に基づいて正しい
                                                                   パケットタイプを選択します。 */
    return (st == kStatus_Success) ? 0 : -1;
}
